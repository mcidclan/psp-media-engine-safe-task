#include <pspctrl.h>
#include <pspdisplay.h>
#include <me-safe-task/me-stask.h>

PSP_MODULE_INFO("stask-demo", 0, 1, 1);
PSP_HEAP_SIZE_KB(-1024);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

void task(void* param) {
  if (param) {
    meCoreDcacheInvalidateRange(param, 64);
    u32* const data = (u32* const)param;
    data[0] += 0x11111111;
    meCoreDcacheWritebackRange(&(data[0]), 64);
    u32 i = 0;
    while (i++ < 0x1000) {
      data[1]++;
      meCoreDcacheWritebackRange(&(data[1]), 64);
    }
  }
}

int main() {
  scePowerSetClockFrequency(333, 333, 166);
  
  pspDebugScreenInit();
  const int error = meSafeTaskInitDispatcher();
  
  if(error < 0) {
    pspDebugScreenPrintf("cant't load kcall prx, error: %i", error);
    sceKernelDelayThread(1000000);
    sceKernelExitGame();
  }
  
  u32* const data = (u32* const)memalign(16, 64);
  data[0] = 0x12345678;
  data[1] = 0;
  sceKernelDcacheWritebackInvalidateRange(data, 64);
  Task meTask = {
    task, data
  };
  meSafeTaskDispatch(&meTask);
  meSafeTaskWaitReady();
  
  SceCtrlData ctl;
  do {
    sceCtrlPeekBufferPositive(&ctl, 1);
    
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("hello");
    pspDebugScreenPrintf("proof0: %lx\n", data[0]);
    pspDebugScreenPrintf("proof1: %lx\n", data[1]);
    
    sceDisplayWaitVblank();
    
  } while (!(ctl.Buttons & PSP_CTRL_HOME));

  sceKernelExitGame();
  return 0;
}
