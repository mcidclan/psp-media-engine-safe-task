#include <pspkernel.h>
#include "me-stask.h"
#include "me-stask-utils.h"
#include "me-stask-kcall.h"

#define CUSTOM_MAGIC_CHECK 0xC0FFEE

#define SYSCALL_CUSTOM_INDEX        0x191
#define SYSCALL_ROUTINE_PATCH_ADDR  0x883004bc
#define SYSCALL_PARAMS_BASE         0xbfc00600

u32 EDRAM_ROUTINE_PATCH_ADDR        = 0;
u32 INTERRUPT_HANDLER_PATCH_ADDR    = 0;
u32 SYSCALL_FUNCTION_PATCH_ADDR     = 0;

unsigned int _ME_SHARED_MEM[16] __attribute__((aligned(64))) = {0};

#define ME_PROCESSING (*_ME_SHARED_MEM)
#define ME_COMMON_PROCESS (1 << 0)
#define ME_CUSTOM_PROCESS (1 << 1)

const u32 ME_PROCESSES = ME_COMMON_PROCESS | ME_CUSTOM_PROCESS; 

#if defined(PRX_FREE) && PRX_FREE

extern int kCall(FCall const f, const unsigned int seg);
extern int kCall(FPCall const f, const unsigned int seg, void* const param);
#else

#define kCall kcall
#endif

static inline int selectTable(void* const param) {
  
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
extern char __start__ipatch_section[];
extern char __stop__ipatch_section[];
extern char __start__fpatch_section[];
extern char __stop__fpatch_section[];

__attribute__((noinline, aligned(4)))
void processPatchedInterruptHandlerRoutine() {
  
  const u32 intr = meCoreInterruptClearMask();
  meCoreDcacheInvalidateRange((void*)&ME_PROCESSING, 64);
  ME_PROCESSING |= ME_COMMON_PROCESS;
  meCoreDcacheWritebackRange((void*)&ME_PROCESSING, 64);
  meCoreInterruptSetMask(intr);
}

__attribute__((noinline, aligned(4)))
int processPatchedEdramRoutine() {
  
  const u32 intr = meCoreInterruptClearMask();
  static u32 init = 0;
  if (!init) {
    //meCoreDcacheWritebackInvalidateAll();
    meSafeIcacheInvalidateAll();
    init = 1;
  }
  return intr;
}

__attribute__((noinline, aligned(4)))
void processPatchedSyscallRoutine() {
  
  u32* const syscall = (u32*)SYSCALL_PARAMS_BASE;
  if (*syscall == SYSCALL_CUSTOM_INDEX) {
    
    const TaskFunc task = (TaskFunc)syscall[2];
    void* const param = (void* const)syscall[3];
    task(param);
  }

  const u32 intr = meCoreInterruptClearMask();
  ME_PROCESSING &= ~ME_PROCESSES;
  meCoreDcacheWritebackRange((void*)&ME_PROCESSING, 64);
  meCoreInterruptSetMask(intr);

  meCoreEmitSoftwareInterrupt();
}

static inline void patchEdramRoutine() {
  
  const u32 target = EDRAM_ROUTINE_PATCH_ADDR;
  meSafeMemcpy((void*)target, (void*)&__start__mpatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
}

static inline void patchSyscallRoutine() {
  
  const u32 target = SYSCALL_ROUTINE_PATCH_ADDR;
  meSafeMemcpy((void*)target, (void*)&__start__patch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
}

static inline void patchInterruptHandlerRoutine() {
  
  const u32 target = INTERRUPT_HANDLER_PATCH_ADDR;
  meSafeMemcpy((void*)target, (void*)&__start__ipatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
}

int patchMeCore() {
  
  patchEdramRoutine();
  patchSyscallRoutine();
  patchInterruptHandlerRoutine();
  return 0;
}

static int waitMeReady() {

  int intr, out = 0;
  while (1) {
    
    intr = sceKernelCpuSuspendIntr();
    sceKernelDcacheInvalidateRange(&ME_PROCESSING, 64);
    if (!(ME_PROCESSING & ME_PROCESSES) || (++out > 500)) { // temporary fix for standby
      break;
    }
    sceKernelCpuResumeIntrWithSync(intr);
    
    sceKernelDelayThread(1000);
  }
  
  intr = sceKernelCpuSuspendIntr();
  
  //ME_PROCESSING = 0;
  ME_PROCESSING &= ~ME_PROCESSES;
  
  sceKernelDcacheWritebackRange(&ME_PROCESSING, 64);
  sceKernelCpuResumeIntrWithSync(intr);
  
  return 0;
}

static inline void triggerSysCall(const u32 index) {
  
  hw(SYSCALL_PARAMS_BASE) = index;
  hw(0xbd000004) = 5;
  meSafeSync();
  hw(0xbc100044) = 1;
  meSafeSync();
  
  //while (!(ME_PROCESSING & ME_COMMON_PROCESS)) {
  //}

  hw(0xbd000004) = 8;
  meSafeSync();
}

void meSafeKernelTaskTriggerCustomProcess() {
  
  const u32 index = hw(SYSCALL_PARAMS_BASE);
  triggerSysCall(index);
}

// static inline 
void meSafeKernelTaskSetCurrentTask(void* task) {
  
  const Task* currentTask = (Task*)task;
  u32* param = (u32*)SYSCALL_PARAMS_BASE;
  param[0] = currentTask->index; // SYSCALL_CUSTOM_INDEX; // -1
  param[2] = (u32)(currentTask->func);
  param[3] = (u32)(currentTask->param);
  param[4] = CUSTOM_MAGIC_CHECK;
  
  int intr = sceKernelCpuSuspendIntr();
  ME_PROCESSING |= ME_CUSTOM_PROCESS;
  sceKernelDcacheWritebackRange(&ME_PROCESSING, 64);
  sceKernelCpuResumeIntrWithSync(intr);

  sceKernelDcacheWritebackInvalidateRange(param, 16);
  sceKernelIcacheInvalidateRange(param, 16);
}

static int dispatchTask(void* task) {
  
  ((Task*)task)->index = SYSCALL_CUSTOM_INDEX;
  meSafeKernelTaskSetCurrentTask(task);
  meSafeKernelTaskTriggerCustomProcess();
  return 0;
}

int meSafeKernelTaskSelectTable() {
  
  return selectTable(NULL);
}

int meSafeTaskSelectTable() {
  
  return meSafeTaskCall(selectTable, NULL);
}

static int init() {
  
  //sceKernelDcacheWritebackInvalidateAll();
  //sceKernelIcacheInvalidateAll();
  
  const int table = meSafeTaskSelectTable();
  switch (table) {
    
    case ME_CORE_T2_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR     = 0x8832162c;
      INTERRUPT_HANDLER_PATCH_ADDR = 0x8838c780;
      SYSCALL_FUNCTION_PATCH_ADDR  = 0x8834f3b0;
      break;
    case ME_CORE_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR     = 0x88312f4c;
      INTERRUPT_HANDLER_PATCH_ADDR = 0x8837b1c0;
      SYSCALL_FUNCTION_PATCH_ADDR  = 0x08365038;
      break;
    case ME_CORE_BL_IMG_TABLE:
      //EDRAM_ROUTINE_PATCH_ADDR        = ;
      //ME_INTERRUPT_HANDLER_PATCH_ADDR = ;//0x883316f8;
      break;
    case ME_CORE_SD_IMG_TABLE:
      //EDRAM_ROUTINE_PATCH_ADDR        = ;
      //ME_INTERRUPT_HANDLER_PATCH_ADDR = ;//0x8832e138;
      break;
    default:
      return -4;
  }
  
  return 0;
}

int meSafeTaskInitDispatcher() {
  
  int error = init();
  if (error) {
    return error;
  }
  
  kCall(patchMeCore, CACHED_KERNEL_MASK);
  return 0;
}

int meSafeTaskDispatch(Task* const task) {
  
  kCall(dispatchTask, CACHED_KERNEL_MASK, task);
  return 0;
}

void meSafeTaskWaitReady() {
  
  kCall(waitMeReady, CACHED_KERNEL_MASK);
}

//
//
// me safe task mini
//
//
__attribute__((noinline, aligned(4)))
void processPatchedSyscallFunction() {
  
  u32* const syscall = (u32*)SYSCALL_PARAMS_BASE;
  if (syscall[4] == CUSTOM_MAGIC_CHECK) {
    const TaskFunc task = (TaskFunc)syscall[2];
    void* const param = (void* const)syscall[3];
    task(param);
    
    const u32 intr = meCoreInterruptClearMask();
    ME_PROCESSING &= ~ME_CUSTOM_PROCESS;
    meCoreDcacheWritebackRange((void*)&ME_PROCESSING, 64);
    meCoreInterruptSetMask(intr);
  }
}

static int patchSyscallFunction() {

  const u32 target = SYSCALL_FUNCTION_PATCH_ADDR;
  meSafeMemcpy((void*)target, (void*)&__start__fpatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
  return 0;
}

static int dispatchTaskMini(void* task) {
  
  ((Task*)task)->index = 36;
  meSafeKernelTaskSetCurrentTask(task);
  meSafeKernelTaskTriggerCustomProcess();
  return 0;
}

int meSafeTaskMiniInit() {
  
  int error = init();
  if (error) {
    return error;
  }
  
  kCall(patchSyscallFunction, CACHED_KERNEL_MASK);
  return 0;
}

int meSafeTaskMiniDispatch(Task* const task) {
  
  return kCall(dispatchTaskMini, CACHED_KERNEL_MASK, task);
}

