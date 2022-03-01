CXX ?= g++
CLL ?= cl
NVCC ?= nvcc
CUDA_PATH ?= /usr/local/cuda-10.0/
OPENMP_PATH = .

ifeq ($(OS),Windows_NT)
	FOPENMP += /openmp
	FOPENMP2 = -fopenmp
	.EXE += .exe
else
	FOPENMP += -fopenmp
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		FOPENMP2 = -Xpreprocessor -fopenmp
	else
		FOPENMP2 = -fopenmp
	endif
endif

LDFLAGS += $(FOPENMP2) -L $(OPENMP_PATH)
ifeq ($(USE_GPU),yes)
ifeq ($(OS),Windows_NT)
	LDFLAGS += -L "$(CUDA_PATH)/lib/x64"
	LIBS += -lcuda -lcudadevrt -lcudart_static
else
	LDFLAGS += -L "$(CUDA_PATH)/lib64/stubs" -L "$(CUDA_PATH)/lib64"
	LIBS += -lcuda -lcudadevrt -lcudart_static -lrt -ldl
endif
endif

CPPFLAGS += -I include
CXXFLAGS += -O3 $(FOPENMP2) -g
ifeq ($(CXX),g++)
	CXXFLAGS += -std=c++11
else
	ifeq ($(UNAME_S),Darwin)
		CXXFLAGS += -std=c++11
		CPPFLAGS += -I /opt/homebrew/include
		LDFLAGS += /opt/homebrew/lib/libomp.dylib
	endif
endif
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
	-rm src/*.o *.obj
	-rm polysolve.exe polysolve
	-rm polysolve.lib polysolve.exp *.pdb

.PHONY: nvcc
nvcc: CPPFLAGS += -DUSE_GPU=1
nvcc: $(SOURCES) $(CU_SOURCES)
	$(NVCC) $(CPPFLAGS) $(NVCCFLAGS) $^ -o polysolve -lcuda

.PHONY: cl
cl: $(SOURCES)
	$(CLL) $(CPPFLAGS) $(CLFLAGS) /Fepolysolve $^
