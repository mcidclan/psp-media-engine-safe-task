#ifndef ME_SAFE_TASK_H
#define ME_SAFE_TASK_H

#include <me-core-mapper/me-core-mapper.h>
#include "me-stask-eshot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunc)(void*);

typedef struct Task {
  TaskFunc func;
  void* param;
  int index;
} Task;

int meSafeTaskInitDispatcher();
int meSafeTaskDispatch(Task* const);
void meSafeTaskWaitReady();

extern void meSafeTaskLoadModule();
extern void meSafeTaskUnloadModule();

// mini
int meSafeTaskMiniInit();
int meSafeTaskMiniDispatch(Task* const task);

// easy shot
extern void meSafeTaskEasyShot(const EasyShot shot);

#ifdef __cplusplus
}
#endif


#endif
