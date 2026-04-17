#ifndef ME_SAFE_TASK_KCALL_H
#define ME_SAFE_TASK_KCALL_H

#include <me-core-mapper/kernel/kcall.h>

#if defined(PRX_FREE)

#ifdef __cplusplus
extern "C" {
#endif

#define _F(_1,_2,_3,NAME,...) NAME
#define kCall(...) _F(__VA_ARGS__, kCall_3, kCall_2, ~)(__VA_ARGS__)
int kCall(FCall const f, const unsigned int seg);
int kCall(FPCall const f, const unsigned int seg, void* const param);

#ifdef __cplusplus
}
#endif

#else

#define kCall kcall

#endif


#endif
