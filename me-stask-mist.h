#ifndef ME_SAFE_TASK_MIST_H
#define ME_SAFE_TASK_MIST_H

#include "me-stask.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MistCfg {
  
  int index;
  u32 addr;
} MistCfg;

int meSafeTaskMistInit();
int meSafeKernelTaskMistInit();
void meSafeTaskMistConfigSyscall (MistCfg* const cfg);
void meSafeKernelTaskMistConfigSyscall(MistCfg* const cfg);
int meSafeTaskMistDispatch(Task* const task);
int meSafeKernelTaskMistDispatch(Task* const task);
void meSafeTaskMistRefreshMe();
void meSafeKernelTaskMistRefreshMe();

#ifdef __cplusplus
}
#endif


#endif

