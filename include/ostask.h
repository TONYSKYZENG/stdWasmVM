#ifndef _OSTASK_H_
#define _OSTASK_H_
#include<stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "fifo32.h"
#include "osconfig.h"
#include "wa.h"
#include "list.h"
/*stores the cpu's running values*/
typedef struct VCPUVARS
{
	//uint32_t fIdx;//the index of current block
   // Saved state
   Block *bStart;
   uint32_t    pc;                // program counter
	uint32_t         sp;                // operand stack pointer
    uint32_t        fp;                // current frame pointer into stack
    uint32_t        csp;   
	uint32_t stackSize,callstackSize,br_tableSize;
    StackValue  *stack; // main operand stack,            // callstack pointer
    Frame       *callstack; // callstack
    uint32_t    *br_table; // br_table branch indexes
}VCPUVARS;

void vcpuvars_getFromM(VCPUVARS *cpuv,Module *m);
void vcpuvars_putToM(VCPUVARS *cpuv,Module *m);
void vcpuvars_fixBlockIndex(VCPUVARS *cpuv,Module *m);
uint8_t *vcpuvars_toNewBin(VCPUVARS *cpuv);
void vcpuvars_toExistBin(VCPUVARS *cpuv,uint8_t *data);
int8_t vcpuvars_newFromFunction(VCPUVARS *cpuv,Module *m,char *funcname);
int8_t vcpuvars_newFromBin(VCPUVARS *cpuv,uint8_t *data,uint32_t dataSize);
uint32_t vcpuvars_binSize(VCPUVARS *cpuv);
void vcpuvars_freeContent(VCPUVARS *cpuv);
void temp_taskSwitch(VCPUVARS *cpuNow,VCPUVARS *cpuSwitchTo,Module *m);
int8_t temp_taskMr_refresh(Module *m);
#define THREAD_MAXLEVEL 5
#define THREAD_ST_IDLE 0
#define THREAD_ST_ACTIVE 1
#define THREAD_ST_RUNNING 2
#define THREAD_ST_WAIT 3
/*define of a thread*/
/*current thread model:
.a module is a task, which is the smallest unit of system resource
.a task may have many threads, sharing the same system resource(eg: global varible)
.thread can be scheduled according to their cpuTicks and level.
.level 0 can alwasys run with the most priority, higher level number must not
run before lower level free, and each thread in the same level can posses the cpu by their
cpuTicks, larger number means more time*/

typedef struct VTLEVEL
{
ELIST threadList;
uint32_t threadCount;
struct VTMR *parent;
}VTLEVEL;
void vtlevel_init(VTLEVEL *vtl);
VTLEVEL *vtlevel_new(void);
typedef struct VTHREAD
{
	VCPUVARS cpuv;//stores the thread's cpu vars
	uint32_t cpuTicks;//determine how many task-switch cycles it can hold
	uint8_t level;
	uint8_t st;
    ELIST levelList;//list member for VTLEVEL TO CONTROL
    ELIST wakeupList;//list member for wake up
	VTLEVEL *vlevelP;
	uint32_t localID;
}VTHREAD;
void vthread_init(VTHREAD *vthread,uint8_t slevel,uint32_t ticks);
VTHREAD *vthread_new(uint8_t slevel,uint32_t ticks);
void vthread_setIdle(VTHREAD *vthread);
void vthread_setActive(VTHREAD *vthread);
int8_t vthread_fillCpuvars(VTHREAD *vthread,Module *m,char *funcname);
typedef struct VTMR
{
	VTLEVEL *runLevel;
	uint8_t allLevels;
	uint8_t runNo;
	VTLEVEL idleLevel;
	VTLEVEL waitLevel;
	VTHREAD *nowThread;
	VTHREAD *nextThread;
	uint32_t tickCount;
}VTMR;
int8_t vtmr_init(VTMR *vtmr,uint8_t allLevel);
VTMR *vtmr_new(uint8_t allLevel);
void vtmr_checkUpdateLevel(VTMR *vtmr);
void vtmr_refreshNoCond(VTMR *vtmr,Module *m);
int8_t vtmr_refresh(VTMR *vtmr,Module *m);
int8_t vthread_addToMr(VTHREAD *vthread,VTMR *vtmr);
int8_t vtmr_endLocal(uint32_t localId,VTMR *vtmr,Module *m);
VTHREAD *vtmr_getLocalThread(VTMR *vtmr,uint32_t localId);
#endif
