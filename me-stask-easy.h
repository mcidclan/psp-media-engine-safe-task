#include <me-safe-task/kcall.h>
#include <me-safe-task/me-stask-mist.h>

#define MIST_SYSCALL_INDEX_13 13

#define MIST_TASK(name, index, param, ...) \
int name(int index, void* param) { \
  if (param) { __VA_ARGS__ } \
  return meSafeTaskMistFinish(); \
}

int meSafeTaskEasyMistRegisterTrigger(MistTrigger* const trigger, const u32 task) {

  static int init = 0;
  
  if (!init) {
    
    init = -1;
    meSafeTaskMistRefreshMe();
    
    const int error = meSafeTaskMistInit();
    if (error >= 0) {
      init = 1;
    }
  }

  if (init > 0) {
    
    MistInjector injector = {
      trigger->index, CACHED_KERNEL_MASK | task
    };
    meSafeTaskMistInjectSyscall(&injector);
    return 0;
  }
  return -1;
}

inline void meSafeTaskEasyMistTrigger(MistTrigger* const trigger) {
  
  meSafeTaskMistTrigger(trigger);
}

inline void meSafeTaskEasyMistTriggerAndWait(MistTrigger* const trigger) {
  
  meSafeTaskMistTrigger(trigger);
  meSafeTaskWaitReady();
}
