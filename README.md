# polysolve

A program to solve polyomino puzzle with multithreading. It is still a work in progress.

## Build

```sh
g++ -O3 -std=c++11 -o polysolve -I include src/CmdArgs.cpp src/main.cpp src/PuzzleReader.cpp src/Puzzle.cpp src/GridType.cpp
```

Or if you are using Visual Studio,

```bat
cl /O2 /EHsc /Fepolysolve -I include src/CmdArgs.cpp src/main.cpp src/PuzzleReader.cpp src/Puzzle.cpp src/GridType.cpp
```
