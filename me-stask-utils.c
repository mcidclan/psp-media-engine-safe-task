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

void meSafeMemcpy (void* const dst, const void* const src, const unsigned int size) {
    
  unsigned int words = size >> 2;
  unsigned int rem   = size & 3;
  unsigned int* d32 = (unsigned int*)dst;
  const unsigned int* s32 = (const unsigned int*)src;
  while (words--) {
    *d32++ = *s32++;
  }
  unsigned char* d8 = (unsigned char*)d32;
  const unsigned char* s8 = (const unsigned char*)s32;
  while (rem--) {
    *d8++ = *s8++;
  }
}

void* meSafeMemset (void* const dst, const int val, unsigned int size) {
  
  unsigned char *d8 = (unsigned char*)dst;
  while (size--) {
    *d8++ = (unsigned char)val;
  }
  return dst;
}

Aligned64 getAligned64Mem(char* const name, int mp, const unsigned int size) {
  
  const int ALIGNMENT = 64;
  SceUID uid = sceKernelAllocPartitionMemory(mp, name, PSP_SMEM_Low, size + ALIGNMENT, NULL);
  u32* mem = (u32*)((((ALIGNMENT - 1) + (unsigned int)sceKernelGetBlockHeadAddr(uid)) & ~(ALIGNMENT - 1)));
  
  return (Aligned64) {
    mem, uid
  };
}
