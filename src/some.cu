#include <iostream>
#include <vector>
__global__ void somecuda(int n, float *gg) {
  int tid=blockDim.x*blockIdx.x+threadIdx.x;
  for(int i=tid;i<n;i+=blockDim.x*gridDim.x) {
    gg[i] = gg[i]*gg[i];
  }
}

struct preCudaInit {
  int numDevices;
  std::vector<int> devices;
  preCudaInit() {
    numDevices = 0;
    if (cudaGetDeviceCount(&numDevices) != cudaSuccess) 
      std::cerr << "Error: Cannot get device count." << '\n';
    else {
      for (int i = 0; i < numDevices; i++) {
        if (cudaSetDevice(i) != cudaSuccess)
          std::cerr << "Error: Cannot switch to device " << i << ".\n";
        else if (cudaFreeArray(nullptr) != cudaSuccess)
          std::cerr << "Error: CUDA context initialization on device " << i << " failed.\n";
        else
          devices.push_back(i);
      }
    }
  }
  ~preCudaInit() {
    ;
  }
};
preCudaInit cucu;

void ss() {
  int *leak;
  cudaMalloc(&leak, 128);
}
