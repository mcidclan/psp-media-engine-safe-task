#ifndef ME_SAFE_TASK_H
#define ME_SAFE_TASK_H

#include <me-core-mapper/me-core-mapper.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*TaskFunc)(void*);

typedef struct Task {
  TaskFunc func;
  void* param;
} Task;

int meSafeTaskInitDispatcher();
int meSafeTaskDispatch(Task* const);
void meSafeTaskWaitReady();
void meSafeLoadModule();
void meSafeUnloadModule();

#ifdef __cplusplus
}
#endif


#endif
