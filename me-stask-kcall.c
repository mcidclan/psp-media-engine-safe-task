#if defined(PRX_FREE) && PRX_FREE

#include <me-core-mapper/kernel/kcall.h>
#include <kubridge.h>

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
  
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

#endif
