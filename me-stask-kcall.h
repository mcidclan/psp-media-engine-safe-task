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

#ifndef ME_SAFE_TASK_KCALL_H
#define ME_SAFE_TASK_KCALL_H

#include <me-core-mapper/kernel/kcall.h>

#ifdef __cplusplus
extern "C" {
#endif

int meSafeTaskCall(FPCall call, void* const param);

#if defined(PRX_FREE)

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
int kCall(FCall const f, const unsigned int seg);
int kCall(FPCall const f, const unsigned int seg, void* const param);
#else

#define kCall kcall
#endif

#ifdef __cplusplus
}
#endif

#endif
