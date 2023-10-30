#include <cstdint>
#include <cub/cub.cuh>
#include <vector>
struct BitMask128 {
  uint64_t x, y;
};
typedef uint32_t ShapeMask;

struct GpuStep {
  BitMask128 *space;
  ShapeMask *unused;
  uint64_t *outpos;
  int *parent;
  int *order;
  int bufsize;
};

struct GpuTempSpace {
  int n_shapes;
  BitMask128 *imagelist;
  int *imgpos;
  int *counts;
  int *tmp_sum;
  size_t tmp_sum_bytes;
  uint64_t *out_pos_size;
};

static_assert(sizeof(ulonglong2) == sizeof(BitMask128), "??");

__device__ int ffs128_gpu(ulonglong2 a) {
  uint64_t nz = a.x;
  int off = 0;
  if (a.x == 0) {
    nz = a.y;
    off = 64;
  }
  return __ffsll(nz) + off - 1;
}

__device__ bool can_fit_gpu(ulonglong2 space, ulonglong2 image) {
  return (space.x & image.x) == image.x && (space.y & image.y) == image.y;
}

__global__ void compute_output_size_gpu(
  ulonglong2 *imagelist, int *imgpos, int n_shapes,
  ulonglong2 *cur_space, ShapeMask *cur_unused,
  int n_cur, int *out_count
) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= n_cur) {
    return;
  }
  ulonglong2 space = cur_space[idx];
  ShapeMask unused = cur_unused[idx];
  int choose = ffs128_gpu(space);
  int cntnext = 0;
  for (int i = 0; i < n_shapes; i++) {
    if (unused >> i & 1) {
      int end = imgpos[choose * n_shapes + i + 1];
      for (int j = imgpos[choose * n_shapes + i]; j < end; j++) {
        ulonglong2 m = imagelist[j];
        if (can_fit_gpu(space, m)) {
          cntnext++;
        }
      }
    }
  }
  out_count[idx] = cntnext;
  printf("out_count[%d]=%d\n", idx, cntnext);
}

__global__ void output_next_gpu(
  ulonglong2 *imagelist, int *imgpos, int n_shapes,
  ulonglong2 *cur_space, ShapeMask *cur_unused, uint64_t *cur_outpos,
  ulonglong2 *next_space, ShapeMask *next_unused, int *next_parent,
  int from, int to
) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  if (idx >= to - from) {
    return;
  }
  idx += from;
  ulonglong2 space = cur_space[idx];
  ShapeMask unused = cur_unused[idx];
  int choose = ffs128_gpu(space);
  int to_insert = idx == 0 ? 0 : cur_outpos[idx-1];
  for (int i = 0; i < n_shapes; i++) {
    if (unused >> i & 1) {
      int end = imgpos[choose * n_shapes + i + 1];
      for (int j = imgpos[choose * n_shapes + i]; j < end; j++) {
        ulonglong2 m = imagelist[j];
        if (can_fit_gpu(space, m)) {
          next_space[to_insert] = make_ulonglong2(space.x ^ m.x, space.y ^ m.y);
          next_unused[to_insert] = unused - (1U<<i);
          next_parent[to_insert] = idx;
          to_insert++;
        }
      }
    }
  }
}

__global__ void search_range_gpu(
  uint64_t *cur_outpos, int from, int to, int buf_size,
  uint64_t out_pos_size[2]
) {
  uint64_t base = from == 0 ? 0 : cur_outpos[from-1];
  int lo = from, hi = to;
  while (lo < hi) {
    int mid = (lo + hi + 1) / 2;
    if (cur_outpos[mid-1] - base > buf_size) {
      hi = mid - 1;
    } else {
      lo = mid;
    }
  }
  out_pos_size[0] = lo;
  if (lo == from) {
    out_pos_size[1] = 0;
  } else {
    out_pos_size[1] = cur_outpos[lo-1] - base;
  }
}

void runGpu(std::vector<GpuStep> g, GpuTempSpace gs) {
  int lv = 0;
  int end = 1;
  int depth = g.size();
  std::vector<int> segStart(depth);
  std::vector<int> segAll(depth);
  std::vector<uint64_t> outposPrev(depth);
  compute_output_size_gpu<<<4000,256>>>((ulonglong2 *) gs.imagelist, gs.imgpos, gs.n_shapes,
    (ulonglong2 *) g[lv].space, g[lv].unused, end, gs.counts);
  cub::DeviceScan::InclusiveSum(gs.tmp_sum, gs.tmp_sum_bytes, gs.counts, g[lv].outpos, end);
  segStart[lv] = 0;
  segAll[lv] = end;
  outposPrev[lv] = 0;
  while (lv < depth) {
    int start = segStart[lv];
    end = segAll[lv];
    uint64_t out_pos_size[2] = {0};
    search_range_gpu<<<1,1>>>(g[lv].outpos, start, end, g[lv].bufsize, gs.out_pos_size);
    cudaMemcpy(out_pos_size, gs.out_pos_size, sizeof(uint64_t[2]), cudaMemcpyDeviceToHost);
    end = out_pos_size[0];
    uint64_t outpos = out_pos_size[1];
    printf("lv.%d start=%d end=%d outpos=%lld\n", lv, start, end, outpos);
    if (end <= start) {
      printf("not enough output space\n");
      return;
    }
    segStart[lv] = end;
    uint64_t outsize = outpos - outposPrev[lv];
    outposPrev[lv] = outpos;
    if (outsize > 0 && lv < depth-1) {
      output_next_gpu<<<4000,256>>>((ulonglong2 *)gs.imagelist, gs.imgpos, gs.n_shapes,
        (ulonglong2 *)g[lv].space, g[lv].unused, g[lv].outpos,
        (ulonglong2 *)g[lv+1].space, g[lv+1].unused, g[lv+1].parent,
        start, end);
      lv++;
      segStart[lv] = 0;
      end = segAll[lv] = outsize;
      compute_output_size_gpu<<<4000,256>>>((ulonglong2 *) gs.imagelist, gs.imgpos, gs.n_shapes,
        (ulonglong2 *) g[lv].space, g[lv].unused, end, gs.counts);
      cub::DeviceScan::InclusiveSum(gs.tmp_sum, gs.tmp_sum_bytes, gs.counts, g[lv].outpos, end);
    } else {
      if (lv == depth-1) {
        printf("ans=%lld\n", outsize);
      }
      while (lv >= 0 && segStart[lv] == segAll[lv]) {
        outposPrev[lv] = 0;
        lv--;
      }
      if (lv == -1) return;
    }
  }
}

int main() {
  FILE *fin = fopen("e.txt", "r");
  if (!fin) {
    printf("no file\n");
    return 1;
  }
  int n_shapes = 0;
  int n_holes = 0;
  char buf[300];
  fscanf(fin, "%d %d", &n_shapes, &n_holes);
  if (n_shapes <= 0 || n_holes <= 0) {
    printf("number of shapes and holes must be > 0\n");
    return 1;
  }
  std::vector<std::vector<BitMask128>> shapelist(128 * n_shapes);
  for (int i = 0; i < n_shapes; i++) {
    int listlen = 0;
    fscanf(fin, "%d", &listlen);
    fgets(buf, 100, fin);
    for (int j = 0; j < listlen; j++) {
      fgets(buf, 200, fin);
      BitMask128 bm = {0, 0};
      int least = -1;
      for (int k = 0; k < n_holes; k++) {
        if (buf[k] == '1') {
          if (least == -1) least = k;
          if (k < 64) bm.x += 1ULL<<k;
          else bm.y += 1ULL<<(k-64);
        }
      }
      if (least != -1) {
        shapelist[least * n_shapes + i].push_back(bm);
      }
    }
  }
  std::vector<BitMask128> flatShapelist;
  std::vector<int> imgpos(shapelist.size() + 1);
  for (int i = 0; i < shapelist.size(); i++) {
    imgpos[i+1] = imgpos[i] + shapelist[i].size();
    for (BitMask128 bm : shapelist[i]) {
      flatShapelist.push_back(bm);
    }
  }

  int bufsize = 5;
  GpuTempSpace gs;
  gs.n_shapes = n_shapes;
  cudaMalloc(&gs.imagelist, sizeof(BitMask128) * flatShapelist.size());
  cudaMalloc(&gs.imgpos, sizeof(int) * imgpos.size());
  cudaMalloc(&gs.counts, sizeof(int) * bufsize);
  uint64_t *dummy = nullptr;
  cub::DeviceScan::InclusiveSum(NULL, gs.tmp_sum_bytes, gs.counts, dummy, bufsize);
  cudaMalloc(&gs.tmp_sum, gs.tmp_sum_bytes);
  cudaMalloc(&gs.out_pos_size, sizeof(uint64_t) * 2);
  printf("prefix sum tmp %lld bytes\n", gs.tmp_sum_bytes);
  cudaMemcpy(gs.imagelist, flatShapelist.data(), flatShapelist.size() * sizeof(BitMask128), cudaMemcpyHostToDevice);
  cudaMemcpy(gs.imgpos, imgpos.data(), imgpos.size() * sizeof(int), cudaMemcpyHostToDevice);

  std::vector<GpuStep> g(n_shapes);
  for (int i = 0; i < n_shapes; i++) {
    cudaMalloc(&g[i].space, sizeof(BitMask128) * bufsize);
    cudaMalloc(&g[i].unused, sizeof(ShapeMask) * bufsize);
    cudaMalloc(&g[i].outpos, sizeof(uint64_t) * bufsize);
    cudaMalloc(&g[i].parent, sizeof(int) * bufsize);
    cudaMalloc(&g[i].order, sizeof(int) * bufsize);
    g[i].bufsize = bufsize;
  }

  BitMask128 space = {0, 0};
  for (int k = 0; k < n_holes; k++) {
    if (k < 64) space.x += 1ULL<<k;
    else space.y += 1ULL<<(k-64);
  }
  ShapeMask unused = (1U<<(n_shapes-1)) * 2 - 1;
  cudaMemcpy(g[0].space, &space, sizeof(BitMask128), cudaMemcpyHostToDevice);
  cudaMemcpy(g[0].unused, &unused, sizeof(ShapeMask), cudaMemcpyHostToDevice);
  runGpu(g, gs);
  printf("%s\n", cudaGetErrorString(cudaDeviceSynchronize()));
}
