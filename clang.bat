rem use clang to build
clang++ -O3 -fopenmp -c -I include -g src/*.cpp
clang++ --cuda-path="%CUDA_PATH%" --cuda-gpu-arch=sm_30 -O3 -c -I include -g src/*.cu
clang++ -L "%CUDA_PATH%\lib\x64" -lcudart_static -lcuda -fopenmp -g -o polysolve.exe *.o
del *.o
