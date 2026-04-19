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
