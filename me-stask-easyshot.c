#include "me-stask-easyshot.h"
#include "me-stask-utils.h"
#include <pspkernel.h>

void meSafeTaskEasyShot(const EasyShot shot) {

  meSafeMemcpy((void*)(0x88300000 + shot.offset), shot.task, shot.size);

  hw(0xbfc00040 + 0x20) = 0x20090003;
  hw(0xbfc00040 + 0x24) = 0x3c048840;
  hw(0xbfc00040 + 0x28) = 0;
  meSafeSync();

  sceKernelDcacheWritebackInvalidateAll();
  sceKernelIcacheInvalidateAll();

  hw(0xbc10004c) |= 0x1434; // 0x14;
  hw(0xbc10004c) = 0x0;
  meSafeSync();
  
  hw(0xBC100070) |= 0x05;
  meSafeSync();
}
