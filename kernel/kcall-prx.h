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
 
#ifndef KCALL_PRX_H
#define KCALL_PRX_H
#ifdef __cplusplus
extern "C" {
#endif
  #define _F(_1,_2,_3,NAME,...) NAME
  #define kCallPrx(...) _F(__VA_ARGS__, kCallPrx_3, kCallPrx_2, ~)(__VA_ARGS__)
  typedef int (*FCall)(void);
  typedef int (*FPCall)(void* const);
  int kCallPrx_2(FCall const func, const unsigned int seg);
  int kCallPrx_3(FPCall const func, const unsigned int seg, void* const param);
#ifdef __cplusplus
}
#endif
#endif
