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
#include "coreneuron/nrniv/nrn_checkpoint.h"
#include "coreneuron/nrnoc/multicore.h"
static int   maxgid;     // no gid in any file can be greater than maxgid
static char* output_dir; // output directory to write simple checkpoint 

void write_phase1(int imult, NrnThread& nt){
// open file for writing
// write nt.n_presyn
// write nt.netcon - nrn_setup_extracon (nrn_setup:390)
// fill array of output_gids with:
// nt_presyns[i]->gid_ - (maxgid * imult);
// TODO can we set net.presyns[i].gid for negtives ? it can help a lot in case we need negatives.
// otherwise: if (nt.presyns[i].ouput_index == -1 => skip
// nc_srcgid[0..nt.n_netcon-1] to write

// close file

}

void write_phase2(int imult, NrnThread& nt){
// open file for writing
// n_outputgid is not stored in read_pahse2
// write nt.ncell
// write nt.end
// write 0 if nt._actual_diam == NULL otherwise write nt.end again
// write number of element in linked list tml (in read file it is nmech)
// for each element in tml:
//        write tml->index
//        write tml->ml->nodecount
// write nt._ndata
// write nt._nidata
// write nt._nvdata
// write nt.n_weight
// write nt._v_parent_index [0 -> nt.end -1]
// write nt._actual_a    [0 -> nt.end -1]
// write nt._actual_b    [0 -> nt.end -1]
// write nt._actual_area [0 -> nt.end -1]
// write nt._actual_v    [0 -> nt.end -1]
// if (nt._actual_diam)
//        write nt._actual_diam [0 -> nt.end]
//
// for each element of tml:
//        if ! nrn_is_artificial [tml->index]
//              write tml->ml->nodeindices [0 -> tml->ml->nodecount -1]
//        for i in [0..ml->nodecount]:
//                                                                  (read care of data layout, here we use 2D loop to always write correct elements)
//              write ml->data[i][0..nrn_prop_param_size[type]]                  (read on line 1129)
//              write ml->pdata[i][0..nrn_dprop_param_size_[type]]               (read on line 1131)
//

// close file
}

void write_phase3(int imult, NrnThread& nt){

}