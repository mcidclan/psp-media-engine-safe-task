#include "me-stask-utils.h"

void meSafeIcacheInvalidateAll() {
  meSafeSync();
  for (int i = 0; i < 8192; i += 64) {
    asm volatile(
        ".set push         \n"
        ".set noreorder    \n"
        "cache 0x04, 0(%0) \n"
        "cache 0x04, 0(%0) \n"
        ".set pop         \n"
        :: "r"(i)
        : "memory"
    );
  }
  meSafeSync();
}

void meSafeMemcpy(void *dst, const void *src, unsigned int size) {
  unsigned int words = size >> 2;
  unsigned int rem   = size & 3;
  unsigned int *d = (unsigned int *)dst;
  const unsigned int *s = (const unsigned int *)src;
  while (words--) {
    *d++ = *s++;
  }
  unsigned char *db = (unsigned char *)d;
  const unsigned char *sb = (const unsigned char *)s;
  while (rem--) {
    *db++ = *sb++;
  }
}
