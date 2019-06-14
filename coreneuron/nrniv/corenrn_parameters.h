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

#ifndef CN_PARAMETERS_H
#define CN_PARAMETERS_H

#include <string>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "CLI/CLI.hpp"

/**
 * \class corenrn_parameters
 * \brief Parses and contains Command Line parameters for Core Neuron
 *
 * This structure contains all the parameters that CoreNeuron fetches
 * from the Command Line. It uses the CLI11 libraries to parse these parameters
 * and saves them in an internal public structure. Each parameter can be
 * accessed or written freely. By default the constructor instantiates a
 * CLI11 object and initializes it for CoreNeuron use.
 * This object is freely accessible from any point of the program.
 * An ostream method is also provided to print out all the parameters that
 * CLI11 parse.
 * Please keep in mind that, due to the nature of the subcommands in CLI11,
 * the command line parameters for subcategories NEED to be come before the relative
 * parameter. e.g. --mpi --gpu gpu --nwarp
 * Also single dash long options are not supported anymore (-mpi -> --mpi).
 */

typedef std::string string_t;

struct corenrn_parameters {

    const int report_buff_size_default=4;

    int spikebuf=100000;           /// Internal buffer used on every rank for spikes
    int prcellgid=-1;              /// Gid of cell for prcellstate
    int ms_phases=2;               /// Number of multisend phases, 1 or 2
    int ms_subint=2;               /// Number of multisend interval. 1 or 2
    int spkcompress=0;             /// Spike Compression
    int cell_interleave_permute=0; /// Cell interleaving permutation
    int nwarp=0;                   /// Number of warps to balance for cell_interleave_permute == 2
    int multiple=1;                /// Model duplication factor
    int extracon=0;                /// Number of extra random connections in each thread to other duplicate models.
    int report_buff_size=report_buff_size_default;    ///Size in MB of the report buffer.
    int seed=0;                   /// Initialization seed for random number generator (int)

    bool mpi_enable=0;            /// Enable MPI flag.
    bool print_arg=0;             /// Print arguments flag.
    bool skip_mpi_finalize=0;     /// Skip MPI finalization
    bool multisend=0;             /// Use Multisend spike exchange instead of Allgather.
    bool threading=0;             /// Enable pthread/openmp
    bool gpu=0;                   /// Enable GPU computation.
    bool binqueue=0;              /// Use bin queue.

    double tstop=100;             /// Stop time of simulation in msec
    double dt=0.025;              /// Timestep to use in msec
    double dt_io=0.1;             /// I/O timestep to use in msec
    double dt_report;             /// I/O timestep to use in msec for reports
    double celsius=34.0;          /// Temperature in degC.
    double voltage=-65.0;         /// Initial voltage used for nrn_finitialize(1, v_init).
    double forwardskip=0.;        /// Forward skip to TIME.
    double mindelay=10.;          /// Maximum integration interval (likely reduced by minimum NetCon delay).

    string_t patternstim;          /// Apply patternstim using the specified spike file.
    string_t datpath=".";          /// Directory path where .dat files
    string_t outpath=".";          /// Directory where spikes will be written
    string_t filesdat="files.dat"; /// Name of file containing list of gids dat files read in
    string_t rconfigfilepath;      /// Read configuration file filename.
    string_t restorepath;          /// Restore simulation from provided checkpoint directory.
    string_t reportfilepath;       /// Reports configuration file.
    string_t checkpointpath;       /// Enable checkpoint and specify directory to store related files.

    CLI::App app{"CoreNeuron - Optimised Simulator Engine for NEURON."};    ///CLI app that performs CLI parsing

    corenrn_parameters();    ///Constructor that initializes the CLI11 app.

    void parse (int argc, char** argv);    /// Runs the CLI11_PARSE macro.

};

std::ostream& operator<<(std::ostream& os, const corenrn_parameters& corenrn_param);    /// Printing method.

extern corenrn_parameters corenrn_param;    /// Declaring global corenrn_parameters object for this instance of CoreNeuron.

#endif //CN_PARAMETERS_H