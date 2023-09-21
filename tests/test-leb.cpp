#include "common.h"

int main() {
  bytedeque bdeq;
  uint64_t k = (uint64_t)72340172838076673;
  encode_i64leb(bdeq, k);
  for (auto &b : bdeq) {
    printf("%2x ", b);
  }
  printf("\n");
  std::vector<byte> kd(bdeq.begin(), bdeq.end());
  ssize_t leblen = 0;
  int64_t val = decode_i64leb(kd.data(), &kd.back() + 1, &leblen);
  printf("%ld\n", val);
  return 0;
}
