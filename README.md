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

Puzzle file is a JSON format file. The top level is an object with the following keys.

* **description**: A string. Description of a puzzle. Optional.
* **grid**: Grid type of the puzzle. Can be one of the following: `square`, `cube`, `triangle`, `hexagon`, and `tan`. Required.
* **board**: A [shape object](#shape-object) describing board shape. Required.
* **shapes**: An array of puzzle pieces. Requires at least one item.
    - **name**: A string. Name of a piece. Optional.
    - **mobility**: Allowed movements of this piece. Can be one of the following: `mirror`, `rotate`, `translate` and `stationary`. Default to `rotate`.
        + `mirror`: The piece can be flipped and rotated.
        + `rotate`: The piece can be rotated, but cannot be flipped.
        + `translate`: The piece can be moved, but cannot be rotated.
        + `stationary`: The piece cannot be moved at all.
    - **amount**: An integer or an object.
    Default value is `1`.
    When it is an object, it has two keys:
        + **min**: Minimum amount
        + **max**: Maximum amount
    - **morphs**: An array of [shape objects](#shape-object) describing possible shapes of a piece. Requires at least one item.
* **notes**: A string. Notes of a puzzle. Optional.

### Shape object
Shape object is a JSON object with either of the two keys: `coords` and `tiles`.

* **coords**: An array of [coordinate tuples](#coordinate-tuple).
* **tiles**: A multi-dimensional array of 0/1 describing the tiles of grid. 0 means absent, 1 means present. The innermost dimension is x axis.

### Coordinate tuple
Coordinate tuple is an array of 1 to 4 integers, and the format is [x, y, z, w].

## Examples

Example puzzle files are in `test/` folder.
* `langford.json`: Compute the number of Langford pairings for N=12.
* `minimum.json`: Smallest valid puzzle file.
* `p01.json`, `p10.json`, `p11.json`, `p20.json`, `p21.json`, `p30.json`, `p31.json`: Subproblems of `pento.json`, to eliminate symmetric solutions of `pento.json`.
* `pento.json`: Fill 6x10 rectangle with pentominoes.
* `soma.json`: Soma cube
* `tangram.json`: Tangram puzzle
* `Y_pento_in_15x15.json`: Pack 45 Y pentominoes in 15x15 square.
* `pento.psv`, `tangram.psv`: PolyForm Puzzle Solver files used to generate `pento.json` and `tangram.json`.

## Converter

You can convert puzzle file created with [PolyForm Puzzle Solver](https://www.jaapsch.net/puzzles/polysolver.htm) by the following command:

```sh
python3 converter/polysolver.py $IN_FILE > $OUT_FILE
```

Currently only the following grid types are supported: Square, Cube, Triangle, Hexagon, and Tan.
