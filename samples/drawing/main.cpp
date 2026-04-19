#include "main.h"

PSP_MODULE_INFO("drawing-demo", 0, 1, 1);
PSP_HEAP_SIZE_KB(-1024);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

#define SHARED_BUF_SIZE 0x88000

volatile u8 SHARED_BUF[SHARED_BUF_SIZE*2] __attribute__((aligned(64))) = {0};

void switchBuf(u32* const buf) {
  *buf ^= SHARED_BUF_SIZE;
  sceDisplaySetFrameBuf((void*)*buf, 512,
    PSP_DISPLAY_PIXEL_FORMAT_8888, PSP_DISPLAY_SETBUF_NEXTFRAME);
  pspDebugScreenSetBase((u32*)*buf);
}

void _meDrawing(void* param) {
  if (param) {
    const u32 data = (u32)param;
    const u32 color = xrand(0xff) << 16 | xrand(0xff) << 8 | xrand(0xff);
    drawLine(data, 512, 128, 64, color);
  }
}

void meDrawing(u32* const buf) {
  Task meTask = {
    _meDrawing, (void*)*buf
  };
  meSafeTaskDispatch(&meTask);
}

void scDrawing(u32* const buf) {
  const u32 color = xrand(0xff) << 16 | xrand(0xff) << 8 | xrand(0xff);
  drawLine((u32)*buf, 512, 16, 64, color);
}

u32 initBuf() {
  const u32 buf = 0x40000000 | (u32)SHARED_BUF;
  pspDebugScreenInitEx((void*)(buf ^ SHARED_BUF_SIZE),
    PSP_DISPLAY_PIXEL_FORMAT_8888, 1);
    
  return buf;
}

int main() {
  scePowerSetClockFrequency(333, 333, 166);
  
  u32 buf = initBuf();
  
  const int error = meSafeTaskInitDispatcher();
  if (error < 0) {
    
    pspDebugScreenPrintf("cant't load kcall prx, error: %i", error);
    exit();
  }
  else {
    meSafeTaskLoadModule();
  }
  
  SceCtrlData ctl;
  do {
    sceCtrlPeekBufferPositive(&ctl, 1);
    
    pspDebugScreenSetXY(0, 0);
    pspDebugScreenPrintf("Shared Drawing Buffer: %lx\n", (u32)buf);
    
    meDrawing(&buf);
    scDrawing(&buf);
    meSafeTaskWaitReady();
    
    switchBuf(&buf);
    sceDisplayWaitVblank();
    
  } while (!(ctl.Buttons & PSP_CTRL_HOME));

  if (error >= 0) {
    meSafeTaskUnloadModule();
  }
  pspDebugScreenPrintf("exiting...");
  exit();
  return 0;
}
