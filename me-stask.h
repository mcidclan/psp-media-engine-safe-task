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
 
#ifndef ME_SAFE_TASK_H
#define ME_SAFE_TASK_H

#include <me-core-mapper/me-core-mapper.h>
#include "me-stask-eshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunc)(void*);

typedef struct Task {
  TaskFunc func;
  void* param;
  int index;
} Task;

int meSafeTaskInitDispatcher();
int meSafeTaskDispatch(Task* const);
void meSafeTaskWaitReady();

extern void meSafeTaskLoadModule();
extern void meSafeTaskUnloadModule();

// mini
int meSafeTaskMiniInit();
int meSafeTaskMiniDispatch(Task* const task);

// easy shot
extern void meSafeTaskEasyShot(const EasyShot shot);

#ifdef __cplusplus
}
#endif


#endif
