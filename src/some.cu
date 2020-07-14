#include <iostream>
#include <cuda.h>
#include <vector>
__global__ void somecuda(int n, float *gg) {
  int tid=blockDim.x*blockIdx.x+threadIdx.x;
  for(int i=tid;i<n;i+=blockDim.x*gridDim.x) {
    gg[i] = gg[i]*gg[i];
  }
}

struct preCudaInit {
  int numDevices;
  std::vector<CUcontext> ctx;
  preCudaInit() {
    numDevices = 0;
    if (cuInit(0) != CUDA_SUCCESS)
      std::cerr << "Error: CUDA initialization failed." << std::endl;
    else if (cuDeviceGetCount(&numDevices) != CUDA_SUCCESS) 
      std::cerr << "Error: Cannot get device count." << '\n';
    else {
      ctx.resize(numDevices);
      for (int i = 0; i < numDevices; i++) {
        CUdevice dev;
        CUdeviceptr ptr;
        if (cuDeviceGet(&dev, i) != CUDA_SUCCESS)
          std::cerr << "Error: Cannot get device " << i << "." << std::endl;
        else if (cuCtxCreate(&ctx[i], 0, dev) != CUDA_SUCCESS)
          std::cerr << "Error: CUDA context creation failed." << std::endl;
        else if (cuMemAlloc(&ptr, 100) != CUDA_SUCCESS)
          std::cerr << "Error: Cannot allocate memory with CUDA Driver API." << std::endl;
        else if (cuMemFree(ptr) != CUDA_SUCCESS)
          std::cerr << "Error: Cannot deallocate memory with CUDA Driver API." << std::endl;
      }
    }
  }
  ~preCudaInit() {
    for (int i = 0; i < numDevices; i++) {
      CUdevice dev;
      if (cuDeviceGet(&dev, 0) != CUDA_SUCCESS)
        std::cerr << "Error: Cannot get device " << i << "." << std::endl;
      else if (cuCtxDestroy(ctx[i]) != CUDA_SUCCESS)
        std::cerr << "Error: CUDA finalization failed." << std::endl;
    }
  }
};
static preCudaInit cucu;

void ss() {
  int *leak;
  cudaMalloc(&leak, 128);
}
