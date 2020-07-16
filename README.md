# polysolve

A program to solve polyomino puzzle with multithreading. It is still a work in progress.

## Build

This program utilizes many C++11 features, so you need a C++11 compiler. If you are using GCC, type the command to compile:

```bash
g++ -O3 -std=c++11 -fopenmp -o polysolve -I include src/*.cpp
```

You need MinGW-w64 build of GCC to compile with GCC on Windows.

Or if you are using Visual Studio 2015,

```Batchfile
cl /O2 /EHsc /openmp /Fepolysolve -I include src/*.cpp
```

Compile with makefile:

* GCC: `make CXX=g++`
* Clang: `make CXX=clang++`
* Clang with GPU: `make CXX=clang++ USE_GPU=yes`
* MSVC: `make cl CLL=cl`
* NVCC: `make nvcc NVCC=nvcc CUDA_PATH=<cuda path>`

## Puzzle file

Puzzle file is a JSON format file. **TODO**: describe file structure.

## Converter

You can convert puzzle file created with [PolyForm Puzzle Solver](https://www.jaapsch.net/puzzles/polysolver.htm) by the following command:

```sh
python3 converter/polysolver.py $IN_FILE > $OUT_FILE
```

Currently only the following grid types are supported: Square, Cube, Triangle, Hexagon, and Tan.
