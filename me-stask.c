#include "me-stask.h"
#include <kubridge.h>

#define SYSCALL_CUSTOM_INDEX        0x191
#define SYSCALL_ROUTINE_PATCH_ADDR  0x883004bc
#define SYSCALL_PARAMS_BASE         0xbfc00600

u32 EDRAM_ROUTINE_PATCH_ADDR        = 0x8832162c;
u32 INTERRUPT_HANDLER_PATCH_ADDR    = 0x8838c878;

unsigned long long _me_processing __attribute__((aligned(64))) = 0;
unsigned long long* me_processing __attribute__((aligned(64))) = NULL;
#define ME_PROCESSING (*me_processing)
#define ME_COMMON_PROCESS (1 << 0)
#define ME_CUSTOM_PROCESS (1 << 1)

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
  
static int kCall(FCall const f, const unsigned int seg) {
  struct KernelCallArg args;
  const unsigned int addr = (seg | (unsigned int)f);
  sceKernelIcacheInvalidateAll();
  return kuKernelCall((void*)addr, &args);
}

static int kCall(FPCall const f, const unsigned int seg, void* const param) {
  struct KernelCallArg args;
  args.arg1 = (u32)param;
  const unsigned int addr = (seg | (unsigned int)f);
  sceKernelIcacheInvalidateAll();
  return kuKernelCall((void*)addr, &args);
}

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
  ME_PROCESSING |= ME_COMMON_PROCESS;
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
  
  meCoreEmitSoftwareInterrupt();

  u32* const syscall = (u32*)SYSCALL_PARAMS_BASE;
  if (*syscall == SYSCALL_CUSTOM_INDEX) {
    
    const TaskFunc task = (TaskFunc)syscall[2];
    void* const param = (void* const)syscall[3];
    task(param);
    ME_PROCESSING &= ~ME_CUSTOM_PROCESS;
  }
  ME_PROCESSING &= ~ME_COMMON_PROCESS;
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

static int patchInterruptHandlerRoutine() {
  
  const u32 target = INTERRUPT_HANDLER_PATCH_ADDR;
  memcpy((void*)target, (void*)&__start__ipatch_section, 8);
  sceKernelDcacheWritebackInvalidateRange((void*)target, 8);
  sceKernelIcacheInvalidateRange((void*)target, 8);
  return 0;
}

static void waitMeReady() {
  
  do {
    if (!ME_PROCESSING) {
      break;
    };
    sceKernelDelayThread(1000);
  } while(1);
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

static int setCurrentTask(void* task) {
  
  const Task* currentTask = (Task*)task;
  u32* param = (u32*)SYSCALL_PARAMS_BASE;
  param[0] = SYSCALL_CUSTOM_INDEX; // -1
  param[2] = (u32)(currentTask->func);
  param[3] = (u32)(currentTask->param);

  ME_PROCESSING |= ME_CUSTOM_PROCESS;
  sceKernelDcacheWritebackInvalidateRange(param, 16);
  sceKernelIcacheInvalidateRange(param, 16);
  return 0;
}

int meSafeTaskInitDispatcher() {
  
  me_processing = (unsigned long long*)(0x40000000 | (u32)(&_me_processing));
  
  sceKernelDcacheWritebackInvalidateAll();
  sceKernelIcacheInvalidateAll();

  // if(meLibLoadPrx() < 0) {
  //  return -3;
  // }
  
  const int table = kCall(selectTable, 0);
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
  
  kCall(patchEdramRoutine, 0);
  kCall(patchSyscallRoutine, 0);
  kCall(patchInterruptHandlerRoutine, 0);
  
  sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
  sceUtilityLoadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
  
  return 0;
}

int meSafeTaskDispatch(Task* const task) {
  
  kCall(setCurrentTask, 0, task);
  kCall(triggerCustomProcess, 0);
  return 0;
}

void meSafeTaskWaitReady() {
  waitMeReady();
}
