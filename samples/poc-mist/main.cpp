#include <pspctrl.h>
#include <pspdisplay.h>
#include <pspkernel.h>
#include <psppower.h>
#include <me-safe-task/kcall.h>
#include <me-safe-task/me-stask-mist.h>

PSP_MODULE_INFO("stask-demo-mist", 0, 1, 1);
PSP_HEAP_SIZE_KB(-1024);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

#define SYSCALL_INDEX 13
u32 meCounter[16] __attribute__((aligned(64))) = {0};

int mistTask(int index, void* param) {
  
  if (param) {
    meCoreDcacheInvalidateRange(param, 64);
    u32* const meCounter = (u32*)param;
    *meCounter += 1;
    meCoreDcacheWritebackRange(param, 64);
  }
  return meSafeTaskMistFinish();
}

int meRunning = 0;
int meThread(SceSize args, void *argp) {
  
  const int error = meSafeTaskMistInit();
  if (error < 0) {
    pspDebugScreenPrintf("cant't load kcall prx, error: %i", error);
    sceKernelDelayThread(1000000);
    sceKernelExitGame();
  }
  
  MistInjector injector = {
    SYSCALL_INDEX, 0x80000000 | (u32)mistTask
  };
  meSafeTaskMistInjectSyscall(&injector);

  MistTrigger trigger = {
    SYSCALL_INDEX, meCounter
  };
  
  while (meRunning) {
    
    meSafeTaskMistTrigger(&trigger);
    meSafeTaskWaitReady();
    sceKernelDcacheWritebackInvalidateAll();
    
    sceKernelDelayThread(1);
  }
  
  return sceKernelExitDeleteThread(0);
}

int main() {
  
  meSafeTaskMistRefreshMe();
  
  scePowerSetClockFrequency(333, 333, 166);
  pspDebugScreenInit();
  
  int thid = sceKernelCreateThread("me-thread", meThread, 0x18, 0x2000, PSP_THREAD_ATTR_VFPU, 0);
  if (thid >= 0) {
    meRunning = 1;
    sceKernelStartThread(thid, 0, 0);
  }
  
  SceCtrlData ctl;
  u32 counter = 0;
  do {
    
    sceCtrlPeekBufferPositive(&ctl, 1);

    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("meCounter: 0x%08lx", *meCounter);
    pspDebugScreenSetXY(0, 1);
    pspDebugScreenPrintf("scCounter: 0x%08lx", counter++);
    
    sceDisplayWaitVblank();
    sceKernelDelayThread(1);
  
  } while (!(ctl.Buttons & PSP_CTRL_HOME));
  
  if (meRunning) {
    
    meRunning = 0;
    SceUInt timeout = 500000;
    int ret = sceKernelWaitThreadEnd(thid, &timeout);
    if (ret < 0) {
      sceKernelTerminateDeleteThread(thid);
    }
  }
  
  sceKernelExitGame();
  return 0;
}
