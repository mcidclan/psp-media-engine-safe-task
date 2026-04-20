#ifndef ME_SAFE_TASK_UTILS_H
#define ME_SAFE_TASK_UTILS_H

#define meSafeSync() asm volatile("sync")

#ifdef __cplusplus
extern "C" {
#endif

void meSafeIcacheInvalidateAll();
void meSafeMemcpy(void *dst, const void *src, unsigned int size);

#ifdef __cplusplus
}
#endif


#endif
