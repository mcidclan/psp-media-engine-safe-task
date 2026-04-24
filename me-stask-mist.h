#ifndef ME_SAFE_TASK_MIST_H
#define ME_SAFE_TASK_MIST_H

#include "me-stask.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MistInjector {
  
  int index;
  u32 addr;
} MistInjector;

typedef struct MistTrigger {
  
  int index;
  void* param;
} MistTrigger;

void meSafeTaskMistRefreshMe();
void meSafeKernelTaskMistRefreshMe();
int  meSafeTaskMistInit();
int  meSafeKernelTaskMistInit();
void meSafeTaskMistInjectSyscall (MistInjector* const injector);
void meSafeKernelTaskMistInjectSyscall(MistInjector* const injector);
int  meSafeTaskMistTrigger(MistTrigger* const trigger);
int  meSafeKernelTaskMistTrigger(MistTrigger* const trigger);
int  meSafeTaskMistFinish();

#ifdef __cplusplus
}
#endif


#endif

