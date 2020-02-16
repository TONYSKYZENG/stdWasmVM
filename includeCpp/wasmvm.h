#ifndef _WASMVM_H_
#define _WASMVM_H_
#ifdef __cplusplus

#else
#define NULL ((void *)0)
#endif
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include<pthread.h>

// Call table/trapping table lookups/execution
#include <signal.h>

#include "util.h"
#include "platform.h"
#include "wa.h"
#include "thunk.h"
#include "system_api.h"

Module *vm_create(char *code, int len);
int vm_exec(Module *m, char *func);
int vm_fuckJump(Module *m, char *func);
int os_apiP3(uint32_t idx,void *p1,void *p2,Module *m);/*the system call used by vos*/
void vm_waitSafe(Module *m);
#include "ostimer.h"
#include "ostask.h"
#include "unitnode.h"
#ifdef __cplusplus
}
#endif
#endif


