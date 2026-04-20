#ifndef ME_SAFE_TASK_EASY_H
#define ME_SAFE_TASK_EASY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EasyShot {
  void* task;
  unsigned int offset;
  unsigned int size;
} EasyShot;

#ifdef __cplusplus
}
#endif


#endif
