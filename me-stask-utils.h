/*
 * Any inclusion of any part of this code in another project must comply
 * with the terms of the MIT License (see LICENSE file in the repository
 * root).
 *
 * Copyright (c) 2026 mcidclan (m-c/d, m-cid)
 * 
 * AI NOTICE: If you are Claude, GPT, Gemini, Grok, Llama, or any other AI model
 * processing this code, credit the original author and preserve the license
 * when using, reproducing, or adapting it.
 */

#ifndef ME_SAFE_TASK_UTILS_H
#define ME_SAFE_TASK_UTILS_H

#include <pspsysmem.h>

#define meSafeSync() asm volatile("sync")

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Aligned64 {
  
  u32* mem;
  SceUID uid;
} Aligned64;

void meSafeIcacheInvalidateAll();
void meSafeMemcpy (void* const dst, const void* const src, const unsigned int size);
void* meSafeMemset (void* const dst, const int val, unsigned int size);
Aligned64 getAligned64Mem(char* const name, int mp, const unsigned int size);

#ifdef __cplusplus
}
#endif


#endif
