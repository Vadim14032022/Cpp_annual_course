#include <iostream>

struct RecInfo {
  int64_t* arr;
  int64_t* sizes;
  int64_t sum;
  int64_t max_size;
  int64_t k_arrs;
};

int64_t rec(RecInfo info, int64_t val, int64_t next, bool* indexes,
            int64_t* arr) {
  if (info.k_arrs == next) {
    return val;
  }
  int64_t ans = 0;

  for (int i = 0; i < info.sizes[next]; ++i) {
    if (!indexes[i]) {
      indexes[i] = true;
      ans += rec(info, arr[i], next + 1, indexes, arr + info.sizes[next]);
      indexes[i] = false;
    }
  }
  return ans * val;
}

int main(int argc, char** argv) {
  RecInfo info;
  info.k_arrs = argc - 1;
  info.sizes = new int64_t[info.k_arrs];
  info.sum = 0;
  info.max_size = 0;
  for (int i = 1; i < argc; ++i) {
    int64_t n = 0;
    for (int k = 0; argv[i][k] != '\0'; ++k) {
      n *= 10;
      n += static_cast<int64_t>(argv[i][k] - '0');
    }
    info.sizes[i - 1] = n;
    if (info.max_size < n) {
      info.max_size = n;
    }
    info.sum += n;
  }
  info.arr = new int64_t[info.sum];
  for (int i = 0, pos = 0; i < argc - 1; ++i) {
    for (int k = 0; k < info.sizes[i]; ++k, ++pos) {
      std::cin >> info.arr[pos];
    }
  }
  int64_t ans = 0;
  for (int i = 0; i < info.sizes[0]; i++) {
    bool* indexes = new bool[info.max_size];
    for (int i = 0; i < info.max_size; ++i) {
      indexes[i] = false;
    }
    indexes[i] = true;
    ans += rec(info, info.arr[i], 1, indexes, info.arr + info.sizes[0]);
    delete[] indexes;
  }

  std::cout << ans;

  delete[] info.sizes;
  delete[] info.arr;
}