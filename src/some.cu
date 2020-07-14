__global__ void somecuda(int n, float *gg) {
  int tid=blockDim.x*blockIdx.x+threadIdx.x;
  for(int i=tid;i<n;i+=blockDim.x*gridDim.x) {
    gg[i] = gg[i]*gg[i];
  }
}

__shared__ int a;
__device__ int r;
