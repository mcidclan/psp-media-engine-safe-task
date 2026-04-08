#include "me-stask.h"

#define SYSCALL_CUSTOM_INDEX        0x191
#define SYSCALL_ROUTINE_PATCH_ADDR  0x883004bc
#define SYSCALL_PARAMS_BASE         0xbfc00600

u32 EDRAM_ROUTINE_PATCH_ADDR        = 0x8832162c;
u32 INTERRUPT_HANDLER_PATCH_ADDR    = 0x8838c878;

unsigned long long ME_PROCESSING __attribute__((aligned(64))) = 0;
#define ME_COMMON_PROCESS (1 << 0)
#define ME_CUSTOM_PROCESS (1 << 1)

#if defined(PRX_FREE) && PRX_FREE
  #include <kubridge.h>

  #define _F(_1,_2,_3,NAME,...) NAME
  #define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
    
  static int kCall(FCall const f, const unsigned int seg) {
    
    struct KernelCallArg args;
    const unsigned int addr = (seg | (unsigned int)f);
    sceKernelIcacheInvalidateAll();
    kuKernelCall((void*)addr, &args);
    return args.ret1;
  }

  static int kCall(FPCall const f, const unsigned int seg, void* const param) {
    
    struct KernelCallArg args;
    args.arg1 = (u32)param;
    const unsigned int addr = (seg | (unsigned int)f);
    sceKernelIcacheInvalidateAll();
    kuKernelCall((void*)addr, &args);
    return args.ret1;
}
#else

  #define kCall kcall
#endif

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
extern char __start__ipatch_section[];
extern char __stop__ipatch_section[];

__attribute__((noinline, aligned(4)))
void processPatchedInterruptHandlerRoutine() {
  
  const u32 intr = meCoreInterruptClearMask();
  meCoreDcacheInvalidateRange((void*)&ME_PROCESSING, 8);
  ME_PROCESSING |= ME_COMMON_PROCESS;
  meCoreDcacheWritebackRange((void*)&ME_PROCESSING, 8);
  meCoreInterruptSetMask(intr);
}

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

  u32* const syscall = (u32*)SYSCALL_PARAMS_BASE;
  if (*syscall == SYSCALL_CUSTOM_INDEX) {
    
    const TaskFunc task = (TaskFunc)syscall[2];
    void* const param = (void* const)syscall[3];
    task(param);
  }

  const u32 intr = meCoreInterruptClearMask();
  ME_PROCESSING = 0;
  meCoreDcacheWritebackRange((void*)&ME_PROCESSING, 8);
  meCoreInterruptSetMask(intr);

  meCoreEmitSoftwareInterrupt();
}

static inline void patchSyscallRoutine() {
  
  const u32 target = SYSCALL_ROUTINE_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__patch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
}

static inline void patchEdramRoutine() {
  
  const u32 target = EDRAM_ROUTINE_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__mpatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
}

static inline void patchInterruptHandlerRoutine() {
  
  const u32 target = INTERRUPT_HANDLER_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__ipatch_section, 8);
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
    sceKernelDcacheInvalidateRange(&ME_PROCESSING, 8);
    if (!ME_PROCESSING || (++out > 1000)) { // temporary fix for standby
      break;
    }
    sceKernelCpuResumeIntrWithSync(intr);
    
    sceKernelDelayThread(1000);
    // meLibDelayPipeline();
  }
  
  intr = sceKernelCpuSuspendIntr();
  ME_PROCESSING = 0;
  sceKernelDcacheWritebackRange(&ME_PROCESSING, 8);
  sceKernelCpuResumeIntrWithSync(intr);
  
  return 0;
}

static inline void triggerSysCall(const u32 index) {
  
  hw(SYSCALL_PARAMS_BASE) = index;
  hw(0xbd000004) = 5;
  meLibSync();
  hw(0xbc100044) = 1;
  meLibSync();
  
  //while (!(ME_PROCESSING & ME_COMMON_PROCESS)) {
  //}

  hw(0xbd000004) = 8;
  meLibSync();
}

static inline void triggerCustomProcess() {
  
  const u32 index = hw(SYSCALL_PARAMS_BASE);
  triggerSysCall(index);
}

static inline void setCurrentTask(void* task) {
  
  const Task* currentTask = (Task*)task;
  u32* param = (u32*)SYSCALL_PARAMS_BASE;
  param[0] = SYSCALL_CUSTOM_INDEX; // -1
  param[2] = (u32)(currentTask->func);
  param[3] = (u32)(currentTask->param);

  int intr = sceKernelCpuSuspendIntr();
  ME_PROCESSING |= ME_CUSTOM_PROCESS;
  sceKernelDcacheWritebackRange(&ME_PROCESSING, 8);
  sceKernelCpuResumeIntrWithSync(intr);

  sceKernelDcacheWritebackInvalidateRange(param, 16);
  sceKernelIcacheInvalidateRange(param, 16);
}

static int dispatchTask(void* task) {
  
  setCurrentTask(task);
  triggerCustomProcess();
  return 0;
}

int meSafeTaskInitDispatcher() {
  
  sceKernelDcacheWritebackInvalidateAll();
  sceKernelIcacheInvalidateAll();

  #if !defined(PRX_FREE) || PRX_FREE == 0

    if(meLibLoadPrx() < 0) {
      return -3;
    }
  #endif
  
  const int table = kCall(selectTable, CACHED_KERNEL_MASK);
  switch(table) {
    
    case ME_CORE_T2_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR     = 0x8832162c;
      INTERRUPT_HANDLER_PATCH_ADDR = 0x8838c780;
      break;
    case ME_CORE_IMG_TABLE:
      EDRAM_ROUTINE_PATCH_ADDR     = 0x88312f4c;
      INTERRUPT_HANDLER_PATCH_ADDR = 0x8837b1c0;
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
  
  kCall(patchMeCore, CACHED_KERNEL_MASK);
  
  sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
  sceUtilityLoadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
  
  return 0;
}

int meSafeTaskDispatch(Task* const task) {
  
  kCall(dispatchTask, CACHED_KERNEL_MASK, task);
  return 0;
}

void meSafeTaskWaitReady() {
  
  kCall(waitMeReady, CACHED_KERNEL_MASK);
}
