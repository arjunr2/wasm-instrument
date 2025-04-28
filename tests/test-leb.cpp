#include "common.h"
#include <assert.h>

void test_i64() {
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

    assert(k == val);
}

void test_i32() {
    bytedeque bdeq;
    uint32_t k = 34128;
    encode_u32leb(bdeq, k);
    for (auto &b : bdeq) {
        printf("%2x ", b);
    }
    printf("\n");

    std::vector<byte> kd(bdeq.begin(), bdeq.end());
    ssize_t leblen = 0;
    int64_t val = decode_u32leb(kd.data(), &kd.back() + 1, &leblen);
    printf("%ld\n", val);

    assert(k == val);
}

int main() {
    test_i64();
    test_i32();
    return 0;
}
