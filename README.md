# polysolve

A program to solve polyomino puzzle with multithreading. It is still a work in progress.

## Build

```bash
g++ -O3 -std=c++11 -o polysolve -I include src/*.cpp
```

Or if you are using Visual Studio 2015,

```Batchfile
cl /O2 /EHsc /Fepolysolve -I include src/*.cpp
```
