# CoreNEURON
> Optimised simulator engine for [NEURON](https://www.neuron.yale.edu/neuron/)

CoreNEURON is a compute engine for the [NEURON](https://www.neuron.yale.edu/neuron/) simulator optimised for both memory usage and computational speed. Its goal is to simulate large cell networks with minimal memory footprint and optimal performance.

If you are a new user and would like to use CoreNEURON, [this tutorial](https://github.com/nrnhines/ringtest) will be a good starting point to understand complete workflow of using CoreNEURON with NEURON.


## Features

CoreNEURON can transparently handle all spiking network simulations including gap junction coupling with the fixed time step method. The model descriptions written in NMODL need to be thread safe to exploit vector units of modern CPUs and GPUs. The NEURON models must use Random123 random number generator.


## Dependencies
* [CMake 3.0.12+](https://cmake.org)
* [MOD2C](http://github.com/BlueBrain/mod2c)
* [MPI 2.0+](http://mpich.org) [Optional]
* [PGI OpenACC Compiler >=18.0](https://www.pgroup.com/resources/accel.htm) [Optional, for GPU systems]
* [CUDA Toolkit >=6.0](https://developer.nvidia.com/cuda-toolkit-60) [Optional, for GPU systems]


## Installation

This project uses git submodules which must be cloned along with the repository itself:

```
git clone --recursive https://github.com/BlueBrain/CoreNeuron.git

```

Set the appropriate MPI wrappers for the C and C++ compilers, e.g.:

```bash
export CC=mpicc
export CXX=mpicxx
```

And build using:

```bash
cmake ..
make -j
```

If you don't have MPI, you can disable MPI dependency using the CMake option `-DCORENRN_ENABLE_MPI=OFF`:

```bash
export CC=gcc
export CXX=g++
cmake .. -DCORENRN_ENABLE_MPI=OFF
make -j
```

And you can run inbuilt tests using:

```
make test
```

### About MOD files

With the latest master branch, the workflow of building CoreNEURON is same as that of NEURON, especially considering the use of **nrnivmodl**. We provide **nrnivmodl-core** for CoreNEURON and you can build **special-core** as:

```bash
/install-path/bin/nrnivmodl-core mod-dir
```


## Building with GPU support

CoreNEURON has support for GPUs using the OpenACC programming model when enabled with `-DCORENRN_ENABLE_GPU=ON`. Below are the steps to compile with PGI compiler:

```bash
module purge
module purge all
module load pgi/18.4 cuda/9.0.176 cmake intel-mpi # change pgi, cuda and mpi modules

export CC=mpicc
export CXX=mpicxx

cmake ..  -DCMAKE_C_FLAGS:STRING="-O2" \
          -DCMAKE_CXX_FLAGS:STRING="-O2" \
          -DCOMPILE_LIBRARY_TYPE=STATIC \
          -DCUDA_HOST_COMPILER=`which gcc` \
          -DCUDA_PROPAGATE_HOST_FLAGS=OFF \
          -DCORENRN_ENABLE_GPU=ON
```

Note that the CUDA Toolkit version should be compatible with PGI compiler installed on your system. Otherwise you have to add extra C/C++ flags. For example, if we are using CUDA Toolkit 9.0 installation but PGI default target is CUDA 8.0 then we have to add :

```bash
-DCMAKE_C_FLAGS:STRING="-O2 -ta=tesla:cuda9.0" -DCMAKE_CXX_FLAGS:STRING="-O2 -ta=tesla:cuda9.0"
```

> If there are large functions / procedures in MOD file that are not inlined by compiler, you need to pass additional c/c++ compiler flags: `-Minline=size:1000,levels:100,totalsize:40000,maxsize:4000`

You have to run GPU executable with the `--gpu` or `-gpu`. Make sure to enable cell re-ordering mechanism to improve GPU performance using `--cell_permute` option (permutation types : 2 or 1):

```bash
mpirun -n 1 ./bin/nrniv-core -d ../tests/integration/ring -mpi -e 100 --gpu --cell_permute 2
```

Note that if your model is using Random123 random number generator, you can't use same executable for CPU and GPU runs. We suggest to build separate executable for CPU and GPU simulations. This will be fixed in future releases.


## Building on Cray System

On a Cray system the user has to provide the path to the MPI library as follows:

```bash
export CC=`which cc`
export CXX=`which CC`
cmake -DMPI_C_INCLUDE_PATH=$MPICH_DIR/include -DMPI_C_LIBRARIES=$MPICH_DIR/lib
make -j
```

## Optimization Flags

* One can specify C/C++ optimization flags specific to the compiler and architecture with `-DCMAKE_CXX_FLAGS` and `-DCMAKE_C_FLAGS` options to the CMake command. For example:

```bash
cmake .. -DCMAKE_CXX_FLAGS="-O3 -g" \
         -DCMAKE_C_FLAGS="-O3 -g" \
         -DCMAKE_BUILD_TYPE=CUSTOM
```

* By default OpenMP threading is enabled. You can disable it with `-DCORENRN_ENABLE_OPENMP=OFF`
* By default CoreNEURON uses the SoA (Structure of Array) memory layout for all data structures. You can switch to AoS using `-DCORENRN_ENABLE_SOA=OFF`.


## RUNNING SIMULATION:

Note that the CoreNEURON simulator dependends on NEURON to build the network model: see [NEURON](https://www.neuron.yale.edu/neuron/) documentation for more information. Once you build the model using NEURON, you can launch CoreNEURON on the same or different machine by:

```bash
export OMP_NUM_THREADS=2     #set appropriate value
mpiexec -np 2 build/bin/nrniv-core -e 10 -d /path/to/model/built/by/neuron -mpi
```

[This tutorial](https://github.com/nrnhines/ringtest) provide more information for parallel runs and performance comparison.

In order to see the command line options, you can use:

```bash
/path/to/isntall/directory/nrniv-core --help
-b, --spikebuf ARG          Spike buffer size. (100000)
-c, --threading             Parallel threads. The default is serial threads.
-d, --datpath ARG           Path containing CoreNeuron data files. (.)
-dt, --dt ARG               Fixed time step. The default value is set by
                            defaults.dat or is 0.025.
-e, --tstop ARG             Stop time (ms). (100)
-f, --filesdat ARG          Name for the distribution file. (files.dat)
-g, --prcellgid ARG         Output prcellstate information for the gid NUMBER.
-gpu, --gpu                 Enable use of GPUs. The default implies cpu only
                            run.
-h, --help                  Print a usage message briefly summarizing these
                            command-line options, then exit.
-k, --forwardskip ARG       Forwardskip to TIME
-l, --celsius ARG           Temperature in degC. The default value is set in
                            defaults.dat or else is 34.0.
-mpi                        Enable MPI. In order to initialize MPI environment
                            this argument must be specified.
-o, --outpath ARG           Path to place output data files. (.)
-p, --pattern ARG           Apply patternstim using the specified spike file.
-R, --cell-permute ARG      Cell permutation, 0 No; 1 optimise node adjacency; 2
                            optimize parent adjacency. (1)
-s, --seed ARG              Initialization seed for random number generator
                            (int).
-v, --voltage ARG           Initial voltage used for nrn_finitialize(1, v_init).
                            If 1000, then nrn_finitialize(0,...). (-65.)
-W, --nwarp ARG             Number of warps to balance. (0)
-x, --extracon ARG          Number of extra random connections in each thread to
                            other duplicate models (int).
--binqueue                  Use bin queue.
--checkpoint ARG            Enable checkpoint and specify directory to store
                            related files.
--mindelay ARG              Maximum integration interval (likely reduced by
                            minimum NetCon delay). (10)
--ms-phases ARG             Number of multisend phases, 1 or 2. (2)
--ms-subintervals ARG       Number of multisend subintervals, 1 or 2. (2)
--multisend                 Use Multisend spike exchange instead of Allgather.
--read-config ARG           Read configuration file filename.
--restore ARG               Restore simulation from provided checkpoint
                            directory.
--show                      Print args.
--skip-mpi-finalize         Do not call mpi finalize.
--spkcompress ARG           Spike compression. Up to ARG are exchanged during
                            MPI_Allgather. (0)
--write-config ARG          Write configuration file filename.
```


## Results

Currently CoreNEURON only outputs spike data as `out.dat` file.

## Running tests

Once you compile CoreNEURON, unit tests and a ring test will be compiled if Boost is available. You can run tests using

```bash
make test
```

If you have different mpi launcher, you can specify it during cmake configuration as:

```bash
cmake .. -DTEST_MPI_EXEC_BIN="mpirun" \
         -DTEST_EXEC_PREFIX="mpirun;-n;2" \
         -DTEST_EXEC_PREFIX="mpirun;-n;2" \
         -DAUTO_TEST_WITH_SLURM=OFF \
         -DAUTO_TEST_WITH_MPIEXEC=OFF \
```
You can disable tests using with options:

```
cmake .. -CORENRN_ENABLE_UNIT_TESTS=OFF
```

## License
* See LICENSE.txt
* See [NEURON](https://www.neuron.yale.edu/neuron/)
* [NMC portal](https://bbp.epfl.ch/nmc-portal/copyright) provides more license information
about ME-type models in testsuite

## Contributors
See [contributors](https://github.com/BlueBrain/CoreNeuron/graphs/contributors).


## Funding

CoreNEURON is developed in a joint collaboration between the Blue Brain Project and Yale University. This work has been funded by the EPFL Blue Brain Project (funded by the Swiss ETH board), NIH grant number R01NS11613 (Yale University), the European Union Seventh Framework Program (FP7/20072013) under grant agreement n◦ 604102 (HBP) and the Eu- ropean Union’s Horizon 2020 Framework Programme for Research and Innovation under Grant Agreement n◦ 720270 (Human Brain Project SGA1) and Grant Agreement n◦ 785907 (Human Brain Project SGA2).
