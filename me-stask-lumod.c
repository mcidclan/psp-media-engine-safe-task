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

#include <pspaudiocodec.h>
#include <psputility_avmodules.h>

void meSafeTaskLoadModule() {
  
  sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
  
  const u32 CODEC_ID = 0x1000;
  unsigned long data[64] __attribute__((aligned(64))) = {0};
  sceAudiocodecGetEDRAM(data, CODEC_ID);
  sceAudiocodecReleaseEDRAM(data);
}

void meSafeTaskUnloadModule() {
  
  sceUtilityUnloadAvModule(PSP_AV_MODULE_AVCODEC);
}
