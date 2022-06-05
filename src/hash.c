//
// Created by Hewston on 6/5/2022.
//

#include <stdint.h>
#include <string.h>

uint64_t siphash24(const void *src, unsigned long src_sz, const char k[16]);

uint64_t hash_key(const char *src) {
    char key[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};
    return siphash24(src, strlen(src), key);
}
