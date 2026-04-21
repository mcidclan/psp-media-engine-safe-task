#ifndef ME_SAFE_TASK_KCALL_H
#define ME_SAFE_TASK_KCALL_H

#include <me-core-mapper/kernel/kcall.h>

#ifdef __cplusplus
extern "C" {
#endif

int meSafeTaskCall(FPCall call, void* const param);

#if defined(PRX_FREE)

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
int kCall(FCall const f, const unsigned int seg);
int kCall(FPCall const f, const unsigned int seg, void* const param);
#else

#define kCall kcall
#endif

#ifdef __cplusplus
}
#endif

#endif
