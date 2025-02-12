/*
Copyright (c) 2016, Blue Brain Project
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "coreneuron/coreneuron.h"
#include "coreneuron/nrnconf.h"
#include "coreneuron/nrnoc/multicore.h"
#include "coreneuron/nrnmpi/nrnmpi.h"
#include "coreneuron/nrnoc/fast_imem.h"
#include "coreneuron/nrnoc/nrnoc_decl.h"
#include "coreneuron/nrniv/nrn_acc_manager.h"
#include "coreneuron/utils/reports/nrnreport.h"
#include "coreneuron/utils/progressbar/progressbar.h"
#include "coreneuron/nrniv/profiler_interface.h"
#include "coreneuron/nrniv/nrn2core_direct.h"

namespace coreneuron {

static void* nrn_fixed_step_thread(NrnThread*);
static void* nrn_fixed_step_group_thread(NrnThread*);

void dt2thread(double adt) { /* copied from nrnoc/fadvance.c */
    if (adt != nrn_threads[0]._dt) {
        int i;
        for (i = 0; i < nrn_nthread; ++i) {
            NrnThread* nt = nrn_threads + i;
            nt->_t = t;
            nt->_dt = dt;
            if (secondorder) {
                nt->cj = 2.0 / dt;
            } else {
                nt->cj = 1.0 / dt;
            }
        }
    }
}

void nrn_fixed_step_minimal() { /* not so minimal anymore with gap junctions */
    if (t != nrn_threads->_t) {
        dt2thread(-1.);
    } else {
        dt2thread(dt);
    }
    nrn_thread_table_check();
    nrn_multithread_job(nrn_fixed_step_thread);
    if (nrn_have_gaps) {
        nrnmpi_v_transfer();
        nrn_multithread_job(nrn_fixed_step_lastpart);
    }
#if NRNMPI
    if (nrn_threads[0]._stop_stepping) {
        nrn_spike_exchange(nrn_threads);
    }
#endif
    t = nrn_threads[0]._t;
}

/* better cache efficiency since a thread can do an entire minimum delay
integration interval before joining
*/
static int step_group_n;
static int step_group_begin;
static int step_group_end;
static progressbar* progress;

void initialize_progress_bar(int nstep) {
    if (nrnmpi_myid == 0) {
        printf("\n");
        progress = progressbar_new(" psolve", nstep);
    }
}

void update_progress_bar(int step, double time) {
    if (nrnmpi_myid == 0) {
        progressbar_update(progress, step, time);
    }
}

void finalize_progress_bar() {
    if (nrnmpi_myid == 0) {
        progressbar_finish(progress);
    }
}

void nrn_fixed_step_group_minimal(int n) {
    static int step = 0;
    dt2thread(dt);
    nrn_thread_table_check();
    step_group_n = n;
    step_group_begin = 0;
    step_group_end = 0;
    initialize_progress_bar(step_group_n);

    while (step_group_end < step_group_n) {
        nrn_multithread_job(nrn_fixed_step_group_thread);
#if NRNMPI
        nrn_spike_exchange(nrn_threads);
#endif

#ifdef ENABLE_REPORTING
        nrn_flush_reports(nrn_threads[0]._t);
#endif
        if (stoprun) {
            break;
        }
        step++;
        step_group_begin = step_group_end;
        update_progress_bar(step_group_end, nrn_threads[0]._t);
    }
    t = nrn_threads[0]._t;
    finalize_progress_bar();
}

static void* nrn_fixed_step_group_thread(NrnThread* nth) {
    int i;
    nth->_stop_stepping = 0;
    for (i = step_group_begin; i < step_group_n; ++i) {
        nrn_fixed_step_thread(nth);
        if (nth->_stop_stepping) {
            if (nth->id == 0) {
                step_group_end = i + 1;
            }
            nth->_stop_stepping = 0;
            return (void*)0;
        }
    }
    if (nth->id == 0) {
        step_group_end = step_group_n;
    }
    return (void*)0;
}

void update(NrnThread* _nt) {
    int i, i1, i2;
    i1 = 0;
    i2 = _nt->end;
#if defined(_OPENACC)
    int stream_id = _nt->stream_id;
#endif
    double* vec_v = &(VEC_V(0));
    double* vec_rhs = &(VEC_RHS(0));

    /* do not need to worry about linmod or extracellular*/
    if (secondorder) {
// clang-format off
        #pragma acc parallel loop present(          \
            vec_v[0:i2], vec_rhs[0:i2])             \
            if (_nt->compute_gpu) async(stream_id)
        // clang-format on
        for (i = i1; i < i2; ++i) {
            vec_v[i] += 2. * vec_rhs[i];
        }
    } else {
// clang-format off
        #pragma acc parallel loop present(              \
                vec_v[0:i2], vec_rhs[0:i2])             \
                if (_nt->compute_gpu) async(stream_id)
        // clang-format on
        for (i = i1; i < i2; ++i) {
            vec_v[i] += vec_rhs[i];
        }
    }

    // update_matrix_to_gpu(_nt);

    if (_nt->tml) {
        assert(_nt->tml->index == CAP);
        nrn_cur_capacitance(_nt, _nt->tml->ml, _nt->tml->index);
    }
    if (nrn_use_fast_imem) { 
        nrn_calc_fast_imem(_nt);
    }
}

void nonvint(NrnThread* _nt) {
    NrnThreadMembList* tml;
    if (nrn_have_gaps) {
        Instrumentor::phase p("gap-v-transfer");
        nrnthread_v_transfer(_nt);
    }
    errno = 0;

    Instrumentor::phase_begin("state-update");
    for (tml = _nt->tml; tml; tml = tml->next)
        if (memb_func[tml->index].state) {
            mod_f_t s = memb_func[tml->index].state;
            std::string ss("state-");
            ss += nrn_get_mechname(tml->index);
            {
                Instrumentor::phase p(ss.c_str());
                (*s)(_nt, tml->ml, tml->index);
            }
#ifdef DEBUG
            if (errno) {
                hoc_warning("errno set during calculation of states", (char*)0);
            }
#endif
        }
    Instrumentor::phase_end("state-update");
}

void nrn_ba(NrnThread* nt, int bat) {
    NrnThreadBAList* tbl;
    for (tbl = nt->tbl[bat]; tbl; tbl = tbl->next) {
        mod_f_t f = tbl->bam->f;
        int type = tbl->bam->type;
        Memb_list* ml = tbl->ml;
        (*f)(nt, ml, type);
    }
}

void nrncore2nrn_send_init() {
    if (nrn2core_trajectory_values_ == nullptr) {
        // standalone execution : no callbacks
        return;
    }
    // if per time step transfer, need to call nrn_record_init() in NEURON.
    // if storing full trajectories in CoreNEURON, need to initialize
    // vsize for all the trajectory requests.
    (*nrn2core_trajectory_values_)(-1, 0, NULL, 0.0);
    for (int tid = 0; tid < nrn_nthread; ++tid) {
        NrnThread& nt = nrn_threads[tid];
        if (nt.trajec_requests) {
            nt.trajec_requests->vsize = 0;
        }
    }
}

void nrncore2nrn_send_values(NrnThread* nth) {
    if (nrn2core_trajectory_values_ == nullptr) {
        // standalone execution : no callbacks
        return;
    }

    TrajectoryRequests* tr = nth->trajec_requests;
    if (tr) {
        if (tr->varrays) {  // full trajectories into Vector data
            double** va = tr->varrays;
            int vs = tr->vsize++;
            assert(vs < tr->bsize);
            for (int i = 0; i < tr->n_trajec; ++i) {
                va[i][vs] = *(tr->gather[i]);
            }
        } else if (tr->scatter) {  // scatter to NEURON and notify each step.
            nrn_assert(nrn2core_trajectory_values_);
            for (int i = 0; i < tr->n_trajec; ++i) {
                *(tr->scatter[i]) = *(tr->gather[i]);
            }
            (*nrn2core_trajectory_values_)(nth->id, tr->n_pr, tr->vpr, nth->_t);
        }
    }
}

static void* nrn_fixed_step_thread(NrnThread* nth) {
    /* check thresholds and deliver all (including binqueue)
       events up to t+dt/2 */
    Instrumentor::phase_begin("timestep");

    {
        Instrumentor::phase p("deliver_events");
        deliver_net_events(nth);
    }

    nth->_t += .5 * nth->_dt;

    if (nth->ncell) {
#if defined(_OPENACC)
        int stream_id = nth->stream_id;
/*@todo: do we need to update nth->_t on GPU: Yes (Michael, but can launch kernel) */
// clang-format off
        #pragma acc update device(nth->_t) if (nth->compute_gpu) async(stream_id)
        #pragma acc wait(stream_id)
// clang-format on
#endif
        fixed_play_continuous(nth);

        {
            Instrumentor::phase p("setup_tree_matrix");
            setup_tree_matrix_minimal(nth);
        }

        {
            Instrumentor::phase p("matrix-solver");
            nrn_solve_minimal(nth);
        }

        {
            Instrumentor::phase p("second_order_cur");
            second_order_cur(nth, secondorder);
        }

        {
            Instrumentor::phase p("update");
            update(nth);
        }
    }
    if (!nrn_have_gaps) {
        nrn_fixed_step_lastpart(nth);
    }
    Instrumentor::phase_end("timestep");
    return (void*)0;
}

void* nrn_fixed_step_lastpart(NrnThread* nth) {
    nth->_t += .5 * nth->_dt;

    if (nth->ncell) {
#if defined(_OPENACC)
        int stream_id = nth->stream_id;
/*@todo: do we need to update nth->_t on GPU */
// clang-format off
        #pragma acc update device(nth->_t) if (nth->compute_gpu) async(stream_id)
        #pragma acc wait(stream_id)
// clang-format on
#endif

        fixed_play_continuous(nth);
        nonvint(nth);
        nrncore2nrn_send_values(nth);
        nrn_ba(nth, AFTER_SOLVE);
        nrn_ba(nth, BEFORE_STEP);
    } else {
        nrncore2nrn_send_values(nth);
    }

    {
        Instrumentor::phase p("deliver_events");
        nrn_deliver_events(nth); /* up to but not past texit */
    }

    return (void*)0;
}
}  // namespace coreneuron
