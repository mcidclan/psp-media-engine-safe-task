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
 
#pragma once

#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <psppower.h>
#include <me-safe-task/me-stask.h>

template<typename T>
inline T xorshift() {
  static T state = 1;
  T x = state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return state = x;
}

inline unsigned short int xrand(const unsigned short int range) {
  unsigned int x = xorshift<unsigned int>();
  unsigned long long m = (unsigned long long)x * (unsigned long long)range;
  return (unsigned short int)(m >> 32);
}

static inline void exit() {
  sceKernelDelayThread(1000000);
  sceKernelExitGame();
}

static inline void drawLine(const u32 buff, const u32 stride,
  const u32 y, const u32 size, const u32 color) {
    
  const u32 base = buff + y * 4 * stride;
  u32* addr = (u32*)base;
  while (addr < (u32 *)(base + (stride*4) * size)) {
    *addr = 0xff000000 | color;
    addr++;
  }
}
