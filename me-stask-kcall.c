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

#include <me-core-mapper/hw-registers.h>
#include "me-stask-kcall.h"

#if defined(PRX_FREE) && PRX_FREE

#include <kubridge.h>

int kCall(FCall const f, const unsigned int seg) {
  
  struct KernelCallArg args;
  const unsigned int addr = (seg | (unsigned int)f);
  sceKernelIcacheInvalidateAll();
  kuKernelCall((void*)addr, &args);
  return args.ret1;
}

int kCall(FPCall const f, const unsigned int seg, void* const param) {
  
  struct KernelCallArg args;
  args.arg1 = (u32)param;
  const unsigned int addr = (seg | (unsigned int)f);
  sceKernelIcacheInvalidateAll();
  kuKernelCall((void*)addr, &args);
  return args.ret1;
}

#else

#include <me-core-mapper/me-lib.h>

static u8 prxLoaded = 0;
#define kCall kcall
#define grabPrx()             \
{                             \
  if (!prxLoaded) {           \
    if (meLibLoadPrx() < 0) { \
      return -3;              \
    }                         \
    prxLoaded = 1;            \
  }                           \
}

#endif

int meSafeTaskCall(FPCall const call, void* const param) {
  
  #if !defined(PRX_FREE) || PRX_FREE == 0
  grabPrx();
  #endif

  return kCall(call, CACHED_KERNEL_MASK, param);
}
