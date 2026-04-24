#include <me-core-mapper/dmacplus.h>
#include "me-stask-utils.h"
#include "me-stask-kcall.h"
#include "me-stask-mist.h"

#define CUSTOM_MAGIC_CHECK    0xC0FFEE
#define SYSCALL_PARAMS_BASE   0xbfc00600

#define ME_PROCESSING         (*_ME_SHARED_MEM)
#define ME_CUSTOM_PROCESS     (1 << 1)

extern int  _ME_SHARED_MEM[16];
extern int  meSafeTaskSelectTable();
extern int  meSafeKernelTaskSelectTable();
extern void meSafeKernelTaskTriggerCustomProcess();

static u32 SYSCALL_TABLE_ADDR = 0; // 0x003ce870 on slim

#if defined(PRX_FREE) && PRX_FREE

extern int kCall(FCall const f, const unsigned int seg);
extern int kCall(FPCall const f, const unsigned int seg, void* const param);
#else

#define kCall kcall
#endif

static int mistRefreshMe (void* param) {
  
  hw(0xBC100050) |= 0x10;
  meSafeSync();
  
  hw(0xbfc00704) =  0x1f;
  meSafeSync();
  
  hw(0xbfc00040 + 0x20) = 0x20090003;
  hw(0xbfc00040 + 0x24) = 0x3c048840;
  hw(0xbfc00040 + 0x28) = 0;
  meSafeSync();
  
  hw(0xbc10004c) |= 0x1434;
  hw(0xbc10004c) = 0x0;
  meSafeSync();
  
  hw(0xBC100070) |= 0x05;
  meSafeSync();
  
  return 0;
}

static int mistInjectSyscall (void* const param) {
    
  MistInjector* injector = (MistInjector*)param;
  const u32 tableBase = SYSCALL_TABLE_ADDR + (injector->index * 4);

  Aligned64 aligned = getAligned64Mem("mist-data-00", PSP_MEMORY_PARTITION_USER, 64);
  aligned.mem[0] = injector->addr;
  sceKernelDcacheWritebackRange(aligned.mem, 64);
  
  int state = sceKernelSuspendDispatchThread();
  cleanSc2MeChannel();
  dmacplusFromSc((u32)aligned.mem, tableBase, DMACPLUS_BYTE_COUNT_4, 1);
  waitSc2MeChannel();
  sceKernelResumeDispatchThread(state);
    
  sceKernelFreePartitionMemory(aligned.uid);
  return 0;
}

static void meSafeKernelTaskMistSetCurrentTrigger(void* const trigger) {
  
  const MistTrigger* currentTrigger = (MistTrigger*)trigger;
  u32* param = (u32*)SYSCALL_PARAMS_BASE;
  param[0] = (u32)currentTrigger->index;
  param[2] = (u32)currentTrigger->index;
  param[3] = (u32)currentTrigger->param;
  param[4] = CUSTOM_MAGIC_CHECK;
  
  int intr = sceKernelCpuSuspendIntr();
  ME_PROCESSING |= ME_CUSTOM_PROCESS;
  sceKernelDcacheWritebackRange(&ME_PROCESSING, 64);
  sceKernelCpuResumeIntrWithSync(intr);

  sceKernelDcacheWritebackInvalidateRange(param, 16);
  sceKernelIcacheInvalidateRange(param, 16);
}

static int mistTriggerTask (void* const trigger) {
  
  meSafeKernelTaskMistSetCurrentTrigger(trigger);
  meSafeKernelTaskTriggerCustomProcess();
  return 0;
}

static int init (const int table) {
  
  switch (table) {
    
    case ME_CORE_T2_IMG_TABLE:
      SYSCALL_TABLE_ADDR           = 0x003ce870;
      break;
    case ME_CORE_IMG_TABLE:
      SYSCALL_TABLE_ADDR           = 0x003ce870; // todo: get/verify syscall table addr for img 
      break;
    case ME_CORE_BL_IMG_TABLE:
      break;
    case ME_CORE_SD_IMG_TABLE:
      break;
    default:
      return -4;
  }
  return 0;
}

// Refresh
void meSafeTaskMistRefreshMe () {
  
  meSafeTaskCall(mistRefreshMe, NULL/*CACHED_KERNEL_MASK*/);
}

void meSafeKernelTaskMistRefreshMe() {
  
  mistRefreshMe(NULL);
}

// Init
int meSafeTaskMistInit () {
  
  const int table = meSafeTaskSelectTable();
  int error = init(table);
  if (error) {
    return error;
  }
  return 0;
}

int meSafeKernelTaskMistInit () {
  
  const int table = meSafeKernelTaskSelectTable();
  int error = init(table);
  if (error) {
    return error;
  }
  return 0;
}

// Inject
void meSafeTaskMistInjectSyscall (MistInjector* const injector) {
  
  kCall(mistInjectSyscall, CACHED_KERNEL_MASK, (void*)injector);
}

void meSafeKernelTaskMistInjectSyscall(MistInjector* const injector) {
  
  mistInjectSyscall((void*)injector);
}

// Trigger
int meSafeTaskMistTrigger (MistTrigger* const trigger) {
  
  
  return kCall(mistTriggerTask, CACHED_KERNEL_MASK, (void*)trigger);
}

int meSafeKernelTaskMistTrigger(MistTrigger* const trigger) {
  
  return mistTriggerTask((void*)trigger);
}

// Finish
int meSafeTaskMistFinish() {
  
  const u32 intr = meCoreInterruptClearMask();
  ME_PROCESSING &= ~ME_CUSTOM_PROCESS;
  meCoreDcacheWritebackRange((void*)_ME_SHARED_MEM, 64);
  meCoreInterruptSetMask(intr);
  return 1;
}
