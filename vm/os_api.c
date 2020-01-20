#include <stdlib.h>
#include <string.h>
#include "wasmvm.h"
VCPUVARS cpuv0,cpuv1;
uint32_t g_taskRunNo=0;

void temp_taskMr_refresh(Module *m)
{
  
 if(g_taskRunNo==0)
   	{
		temp_taskSwitch(&cpuv0,&cpuv1,m);
		g_taskRunNo=1;
	

   	}
   else if(g_taskRunNo==1)
   	{

		temp_taskSwitch(&cpuv1,&cpuv0,m);
		g_taskRunNo=0;
		
   	}
   interpret(m);
   
		

}

void os_apiIdx0(void *p1,Module *m)
{
char amsg[64];
uint32_t p=(uint32_t)p1;
 strcpy(amsg, (m->memory.bytes + p));
printf("%s\n", amsg);

}
void os_apiIdx1(void *p1)
{
uint32_t p=(uint32_t)p1;
printf("%d\n", p);

}
void os_CpuNewThread(Module *m,void *p1)
{
	  int fidx = 0, res = 0;
	    Block  *func ;
    Type   *type;
	char amsg[64];
uint32_t p=(uint32_t)p1;
 strcpy(amsg, (m->memory.bytes + p));
	printf("creat task in %s",amsg);
   fidx = get_export_fidx(m, amsg);
	VCPUVARS *cpuvx=&cpuv1;
    if (fidx == -1) {
	FATAL("no exported function %s\n", func);
	return ;
    }
	func = &m->functions[fidx];
	type = func->type;
	cpuvx->pc = func->start_addr;
	cpuvx->block=func;
    cpuvx->sp  =0;
    cpuvx->fp  = 0;
    cpuvx->csp = 0;
	 cpuvx->stack=(StackValue *)calloc(1,STACK_SIZE*sizeof(StackValue));
  if(cpuvx->stack==0)
  	{
	  error("memory error,no vm stack is allocated!\r\n");

  return 0;
  	}
  
  cpuvx->callstack=(Frame *)calloc(1,CALLSTACK_SIZE*sizeof(Frame));
  if(cpuvx->callstack==0)
  	{
  	
	error("memory error,no vm callstack is allocated!\r\n");
	  return 0;

  	}
  cpuvx->br_table=(uint32_t *)calloc(1,BR_TABLE_SIZE*sizeof(uint32_t));
  if(cpuvx->br_table==0)
  	{
  	
	error("memory error,no vm branch table is allocated!\r\n");
		  return 0;
  	}
  /**/
  /*for (uint32_t lidx=0; lidx<func->local_count; lidx++) {
        cpuvx->sp += 1;
        cpuvx->stack[cpuvx->sp].value_type = func->locals[lidx];
        cpuvx->stack[cpuvx->sp].value.uint64 = 0; // Initialize whole union to 0
    }*/
   /* cpuvx->callstack[cpuvx->csp].block = func;
    cpuvx->callstack[cpuvx->csp].sp = cpuvx->sp;
    cpuvx->callstack[cpuvx->csp].fp = cpuvx->fp;
    cpuvx->callstack[cpuvx->csp].ra = cpuvx->pc;*/
  cpuvx->stackSize=STACK_SIZE;
  cpuvx->callstackSize=CALLSTACK_SIZE;
  cpuvx->br_tableSize=BR_TABLE_SIZE;
  // vcpuvars_getFromM(&cpuv0,m);*/
    return 0;
	//interpret(m);
//	vcpuvars_getFromM(&cpuv0,m);
}
void os_popCpu(Module *m)
{
	 
	//interpret(m);
	vcpuvars_putToM(&cpuv0,m);
}
void os_sleep(void *p1)
{
	uint32_t p=(uint32_t)p1;
	sleep(p);

}


int os_apiP3(uint32_t idx,void *p1,void *p2,Module *m)
{
switch (idx){
	case 0:os_apiIdx0(p1,m);break;
	case 1:os_apiIdx1(p1);break;
	case 2:os_CpuNewThread(m,p1);break;
	case 3:os_popCpu(m);break;
    case 4:os_sleep(p1);break;






}
return 0;
}

