//#include <pspkernel.h>

#include <me-core-mapper/dmacplus.h>
#include "me-stask-utils.h"
#include "me-stask-kcall.h"
#include "me-stask-mist.h"

extern int meSafeTaskSelectTable();
extern int meSafeKernelTaskSelectTable();

extern void meSafeKernelTaskSetCurrentTask(void* task);
extern void meSafeKernelTaskTriggerCustomProcess();

//static 
static int MIST_SYSCALL_INDEX = 0; // 12;
static u32 SYSCALL_TABLE_ADDR = 0x003ce870; // slim

#if defined(PRX_FREE) && PRX_FREE

extern int kCall(FCall const f, const unsigned int seg);
extern int kCall(FPCall const f, const unsigned int seg, void* const param);
#else

#define kCall kcall
#endif

static int mistRefreshMe (void* param) {
  
  hw(0xbfc00704) = 0xffffffff;
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

static int mistConfigSyscall (void* const param) {
  
  MistCfg* cfg = (MistCfg*)param;
  
  MIST_SYSCALL_INDEX = cfg->index;
  const u32 tableBase = SYSCALL_TABLE_ADDR + (MIST_SYSCALL_INDEX * 4);

  cleanSc2MeChannel();
  dmacplusFromSc(cfg->addr, tableBase, DMACPLUS_BYTE_COUNT_4, 1);
  waitSc2MeChannel();
  
  return 0;
}

static int mistDispatchTask (void* task) {
  
  ((Task*)task)->index = MIST_SYSCALL_INDEX;
  meSafeKernelTaskSetCurrentTask(task);
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

// Config
void meSafeTaskMistConfigSyscall (MistCfg* const cfg) {
  
  kCall(mistConfigSyscall, CACHED_KERNEL_MASK, (void*)cfg);
}

void meSafeKernelTaskMistConfigSyscall(MistCfg* const cfg) {
  
  mistConfigSyscall((void*)cfg);
}

// Dispatch
int meSafeTaskMistDispatch (Task* const task) {
  
  if (!MIST_SYSCALL_INDEX) {
    return -1;
  }
  task->index = MIST_SYSCALL_INDEX;
  return kCall(mistDispatchTask, CACHED_KERNEL_MASK, task);
}

int meSafeKernelTaskMistDispatch(Task* const task) {
  
  if (!MIST_SYSCALL_INDEX) {
    return -1;
  }
  task->index = MIST_SYSCALL_INDEX;
  return mistDispatchTask(task);
}

// Refresh
void meSafeTaskMistRefreshMe () {
  
  meSafeTaskCall(mistRefreshMe, NULL/*CACHED_KERNEL_MASK*/);
}

void meSafeKernelTaskMistRefreshMe() {
  
  mistRefreshMe(NULL);
}
