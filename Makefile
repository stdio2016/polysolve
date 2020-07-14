CXX ?= clang++
CUDA_PATH ?= /usr/local/cuda-10.0/
OPENMP_PATH = 

ifeq ($(OS),Windows_NT)
	FOPENMP += /openmp
	.EXE += .exe
else
	FOPENMP += -fopenmp
endif

ifeq ($(OS),Windows_NT)
	LDFLAGS += -fopenmp -L "$(CUDA_PATH)/lib/x64"
	LIBS += -lcuda -lcudadevrt -lcudart_static
else
	LDFLAGS += -fopenmp -L $(OPENMP_PATH) -L "$(CUDA_PATH)/lib64/stubs" -L "$(CUDA_PATH)/lib64"
	LIBS += -lcuda -lcudadevrt -lcudart_static -lrt -ldl
endif

CXXFLAGS += -O3 -fopenmp -c -I include -I /usr/lib/gcc/x86_64-linux-gnu/7/include -g
CUFLAGS += --cuda-path="$(CUDA_PATH)" --cuda-gpu-arch=sm_30 -O3 -I include

SOURCES := $(wildcard src/*.cpp)
CU_SOURCES := $(wildcard src/*.cu)

polysolve$(.EXE): $(SOURCES:%.cpp=%.o) $(CU_SOURCES:%.cu=%.o)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

src/%.o: src/%.cpp Makefile $(wildcard src/*.hpp)
	$(CXX) $(CXXFLAGS) $< -c -o $@

src/some.o: src/some.cu Makefile $(wildcard src/*.hpp)
	$(CXX) $(CUFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm src/*.o
	-rm src/*.o2
	-rm polysolve.exe
	-rm polysolve

.PHONY: nvcc
nvcc: $(SOURCES) $(CU_SOURCES)
	$(CXX) -O3 -I include -Xcompiler $(FOPENMP) $^ -o polysolve -lcuda
