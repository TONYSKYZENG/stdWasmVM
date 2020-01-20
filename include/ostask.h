#ifndef _OSTASK_H_
#define _OSTASK_H_
#include<stdio.h>
#include <stdint.h>
#include "fifo32.h"
#include "osconfig.h"
#include "wa.h"
/*stores the cpu's running values*/
typedef struct VCPUVARS
{
Block	   *block;
   // Saved state
   uint32_t    pc;                // program counter
    int         sp;                // operand stack pointer
    int         fp;                // current frame pointer into stack
    StackValue  *stack; // main operand stack,
    uint32_t stackSize,callstackSize,br_tableSize;
    int         csp;               // callstack pointer
    Frame       *callstack; // callstack
    uint32_t    *br_table; // br_table branch indexes
}VCPUVARS;
void vcpuvars_getFromM(VCPUVARS *cpuv,Module *m);
void vcpuvars_putToM(VCPUVARS *cpuv,Module *m);
void temp_taskSwitch(VCPUVARS *cpuNow,VCPUVARS *cpuSwitchTo,Module *m);
void temp_taskMr_refresh(Module *m);
#endif
