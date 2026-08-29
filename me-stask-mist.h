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
 
#ifndef ME_SAFE_TASK_MIST_H
#define ME_SAFE_TASK_MIST_H

#include "me-stask.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MistInjector {
  
  int index;
  u32 addr;
} MistInjector;

typedef struct MistTrigger {
  
  int index;
  void* param;
} MistTrigger;

void meSafeTaskMistRefreshMe();
void meSafeKernelTaskMistRefreshMe();
int  meSafeTaskMistInit();
int  meSafeKernelTaskMistInit();
void meSafeTaskMistInjectSyscall (MistInjector* const injector);
void meSafeKernelTaskMistInjectSyscall(MistInjector* const injector);
int  meSafeTaskMistTrigger(MistTrigger* const trigger);
int  meSafeKernelTaskMistTrigger(MistTrigger* const trigger);
int  meSafeTaskMistFinish();

#ifdef __cplusplus
}
#endif


#endif

