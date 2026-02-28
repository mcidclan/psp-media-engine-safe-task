#include "me-stask.h"

static volatile int step = 0;

#define SYSCALL_CUSTOM_INDEX        0x191
#define SYSCALL_ROUTINE_PATCH_ADDR  0x883004bc
#define SYSCALL_PARAMS_BASE         0xbfc00600

u32 EDRAM_ROUTINE_PATCH_ADDR = 0x8832162c;

static inline int selectTable() {
  
  const int tableId = meCoreGetTableIdFromWitnessWord();
  if (tableId < 2) {
    return -1;
  }
  meCoreSelectSystemTable(tableId);
  return tableId;
}

extern char __start__patch_section[];
extern char __stop__patch_section[];
extern char __start__mpatch_section[];
extern char __stop__mpatch_section[];

__attribute__((noinline, aligned(4)))
int processPatchedEdramRoutine() {
  
  const u32 intr = meCoreInterruptClearMask();
  static u32 init = 0;
  if (!init) {
    meLibIcacheInvalidateAll();
    init = 1;
  }
  return intr;
}

__attribute__((noinline, aligned(4)))
void processPatchedSyscallRoutine() {
  
  meCoreEmitSoftwareInterrupt();

  const u32* const syscall = (u32*)SYSCALL_PARAMS_BASE;
  if (*syscall == SYSCALL_CUSTOM_INDEX) {
    const TaskFunc task = (TaskFunc)syscall[2];
    void* const param = (void* const)syscall[3];
    task(param);
  }
}

static int patchSyscallRoutine() {
  
  const u32 target = SYSCALL_ROUTINE_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__patch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
  return 0;
}

static int patchEdramRoutine() {
  
  const u32 target = EDRAM_ROUTINE_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__mpatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
  return 0;
}

static inline void triggerSysCall(const u32 index) {
  
  hw(SYSCALL_PARAMS_BASE) = index;
  hw(0xbd000004) |= 5;
  meLibSync();
  hw(0xbc100044) = 1;
  meLibSync();
}

static int triggerCustomProcess() {
  
  const u32 index = hw(SYSCALL_PARAMS_BASE);
  triggerSysCall(index);
  return 0;
}


Task* currentTask = NULL;

static int setCurrentTask() {
  
  u32* param = (u32*)SYSCALL_PARAMS_BASE;
  param[0] = SYSCALL_CUSTOM_INDEX; // -1
  param[2] = (u32)(currentTask->func);
  param[3] = (u32)(currentTask->param);
  sceKernelDcacheWritebackInvalidateRange(param, 16);
  sceKernelIcacheInvalidateRange(param, 16);
  return 0;
}

__attribute__((noinline, aligned(4)))
void checkReady(void* param) {
  
  if (param) {
    
    u32* const meSafeTaskDispatcherReady = (u32* const)param;
    meCoreDcacheInvalidateRange(&meSafeTaskDispatcherReady[0], 64);
    //const u32 intr = meCoreInterruptClearMask();
    meSafeTaskDispatcherReady[0] = 1;
    //meCoreInterruptSetMask(intr);
    meCoreDcacheWritebackRange(&meSafeTaskDispatcherReady[0], 64);
  }
}

int meSafeTaskInitDispatcher() {
  
  sceKernelDcacheWritebackInvalidateAll();
  sceKernelIcacheInvalidateAll();

  if(meLibLoadPrx() < 0) {
    return -3;
  }
  
  const int table = kcall(selectTable, 0);
  switch(table) {
    
    case ME_CORE_T2_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR = 0x8832162c;
      break;
    case ME_CORE_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR = 0x88312f4c;
      break;
      
    case ME_CORE_BL_IMG_TABLE:
    case ME_CORE_SD_IMG_TABLE:
    default:
      return -4;
  }
  
  kcall(patchEdramRoutine, 0);
  kcall(patchSyscallRoutine, 0);
  
  sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
  sceUtilityLoadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
  
  return 0;
}

int meSafeTaskDispatch(Task* const task) {
  
  currentTask = task;
  kcall(setCurrentTask, 0);
  kcall(triggerCustomProcess, 0);
  return 0;
}

typedef struct {
  volatile u32 value;
  u8 _pad[60];
} __attribute__((aligned(64))) TaskDispatcherState;

void meSafeTaskWaitReady() {
 
  static volatile TaskDispatcherState meSafeTaskDispatcherReady __attribute__((aligned(64))) = {0};
  
  meSafeTaskDispatcherReady.value = 0;
  sceKernelDcacheWritebackInvalidateRange((void*)&meSafeTaskDispatcherReady, 4);
  
  Task meTask = {
    checkReady, (void*)&meSafeTaskDispatcherReady
  };
  meSafeTaskDispatch(&meTask);
  
  do {
    sceKernelDelayThread(1000);
    sceKernelDcacheInvalidateRange((void*)&meSafeTaskDispatcherReady, 4);
  } while (!meSafeTaskDispatcherReady.value);
}
