#ifndef ME_SAFE_STACK_H
#define ME_SAFE_STACK_H

#include <me-core-mapper/me-core-mapper.h>
#include <me-core-mapper/me-lib.h>

#include <pspaudiocodec.h>
#include <psputility_avmodules.h>

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

#ifdef __cplusplus
}
#endif

#endif
