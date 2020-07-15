CXX ?= clang++
CLL ?= cl
NVCC ?= nvcc
CUDA_PATH ?= /usr/local/cuda-10.0/
OPENMP_PATH = .

ifeq ($(OS),Windows_NT)
	FOPENMP += /openmp
	.EXE += .exe
else
	FOPENMP += -fopenmp
endif

LDFLAGS += -fopenmp -L $(OPENMP_PATH)
ifeq ($(USE_GPU),yes)
ifeq ($(OS),Windows_NT)
	LDFLAGS += -L "$(CUDA_PATH)/lib/x64"
	LIBS += -lcuda -lcudadevrt -lcudart_static
else
	LDFLAGS += -L "$(CUDA_PATH)/lib64/stubs" -L "$(CUDA_PATH)/lib64"
	LIBS += -lcuda -lcudadevrt -lcudart_static -lrt -ldl
endif
endif

CPPFLAGS += -I include -I /usr/lib/gcc/x86_64-linux-gnu/7/include
CXXFLAGS += -O3 -fopenmp -g
CUFLAGS += --cuda-path="$(CUDA_PATH)" --cuda-gpu-arch=sm_30 -O3 -g
NVCCFLAGS += -Xcompiler $(FOPENMP) -O3 -lineinfo -g
CLFLAGS += /O2 /EHsc /openmp

SOURCES := $(wildcard src/*.cpp)
CU_SOURCES := $(wildcard src/*.cu)
ifeq ($(USE_GPU),yes)
	CPPFLAGS += -DUSE_GPU=1
	CUcu_SOURCES :=$(CU_SOURCES)
endif

polysolve$(.EXE): $(SOURCES:%.cpp=%.o) $(CUcu_SOURCES:%.cu=%.o)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

src/%.o: src/%.cpp Makefile $(wildcard src/*.hpp)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< -c -o $@

src/some.o: src/some.cu Makefile $(wildcard src/*.hpp)
	$(CXX) $(CPPFLAGS) $(CUFLAGS) $< -c -o $@

.PHONY: clean
clean:
	-rm src/*.o
	-rm src/*.o2
	-rm polysolve.exe
	-rm polysolve

.PHONY: nvcc
nvcc: CPPFLAGS += -DUSE_GPU=1
nvcc: $(SOURCES) $(CU_SOURCES)
	$(NVCC) $(CPPFLAGS) $(NVCCFLAGS) $^ -o polysolve -lcuda

.PHONY: cl
cl: $(SOURCES)
	$(CLL) $(CPPFLAGS) $(CLFLAGS) /Fepolysolve $^
