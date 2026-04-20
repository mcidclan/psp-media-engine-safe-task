#include "me-stask-eshot.h"
#include "me-stask-utils.h"

#include <me-core-mapper/kernel/kcall.h>
#include <me-core-mapper/hw-registers.h>
#include <pspkernel.h>

#if defined(PRX_FREE) && PRX_FREE

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
extern int kCall(FCall const f, const unsigned int seg);
extern int kCall(FPCall const f, const unsigned int seg, void* const param);

#else

#define kCall kcall

#endif

static int kernelEasyShot(void* _shot) {
  
  if (!_shot) {
    return -1;
  }
  
  EasyShot* const shot = (EasyShot*)_shot;
  if (!shot->size) {
    return -1;
  }
  
  meSafeMemcpy((void*)(0x88300000 + shot->offset), shot->task, shot->size);

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

  return 0;
}

void meSafeTaskEasyShot(const EasyShot shot) {
  kCall(kernelEasyShot, CACHED_KERNEL_MASK, (void*)&shot);
}
