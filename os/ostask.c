#include "ostask.h"

/*name: vcpuvars_getFromM
description:get the cpu's running vars from a module
input :VCPUVARS *cpuv,Module *m
output :NULL
date:20200119*/
void vcpuvars_getFromM(VCPUVARS *cpuv,Module *m)
{

cpuv->block=m->runCB;
cpuv->fp=m->fp;
cpuv->sp=m->sp;
cpuv->pc=m->pc;
cpuv->csp=m->csp;
cpuv->stack=m->stack;
cpuv->callstack=m->callstack;
cpuv->br_table=m->br_table;
cpuv->stackSize=m->stackSize;
cpuv->callstackSize=m->callstackSize;
cpuv->br_tableSize=m->br_tableSize;

}
/*name: vcpuvars_putToM
description:put a stored cpu's running vars to a module
input :VCPUVARS *cpuv,Module *m
output :NULL
date:20200119*/

void vcpuvars_putToM(VCPUVARS *cpuv,Module *m)
{
	m->runCB=cpuv->block;
	m->fp=cpuv->fp;
	m->sp=cpuv->sp;
	m->pc=cpuv->pc;
  m->stack=cpuv->stack;
  m->csp=cpuv->csp;
    m->callstack=cpuv->callstack;
	m->br_table=cpuv->br_table;
    m->stackSize=cpuv->stackSize;
    m->callstackSize=cpuv->callstackSize;
    m->br_tableSize=cpuv->br_tableSize;
}
void temp_taskSwitch(VCPUVARS *cpuNow,VCPUVARS *cpuSwitchTo,Module *m)
{
	vcpuvars_getFromM(cpuNow,m);
	//cpuNow->pc-=1;
	vcpuvars_putToM(cpuSwitchTo,m);

}

