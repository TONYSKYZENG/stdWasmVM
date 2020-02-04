#include "ostask.h"
#include "wa.h"
#include "util.h"


/*name: vcpuvars_getFromM
description:get the cpu's running vars from a module
input :VCPUVARS *cpuv,Module *m
output :NULL
date:20200120*/
void vcpuvars_getFromM(VCPUVARS *cpuv,Module *m)
{

//cpuv->block=m->runCB;
//cpuv->fIdx=m->runIdx;
cpuv->bStart=m->localBlockStart;
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
date:20200120*/
void vcpuvars_putToM(VCPUVARS *cpuv,Module *m)
{
	//m->runCB=cpuv->block;
	//m->runIdx=cpuv->fIdx;
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

/*name:vcpuvars_newFromFunction
description:establish a new VCPUVARS from a module's function by its name
input :VCPUVARS *cpuv,Module *m,char *funcname
output :int8_t, 1 for success
date:20200121*/
int8_t vcpuvars_newFromFunction(VCPUVARS *cpuv,Module *m,char *funcname)
{
	uint32_t fidx = 0, res = 0;
			Block  *func ;
		Type   *type;
	
	   fidx = get_export_fidx(m, funcname);
		VCPUVARS *cpuvx=cpuv;
		if (fidx == -1) {
		FATAL("no exported function %s\n", func);
		return -1;
		}
		func = &m->functions[fidx];
		type = func->type;
		cpuvx->pc = func->start_addr;
		//cpuvx->block=func;
		cpuvx->sp  =0;
		cpuvx->fp  = 0;
		cpuvx->csp = 0;
		cpuvx->bStart=m->localBlockStart;
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
	  for (uint32_t lidx=0; lidx<func->local_count; lidx++) {
			cpuvx->sp += 1;
			cpuvx->stack[cpuvx->sp].value_type = func->locals[lidx];
			cpuvx->stack[cpuvx->sp].value.uint64 = 0; // Initialize whole union to 0
		}
	    cpuvx->callstack[cpuvx->csp].block = func;
		cpuvx->callstack[cpuvx->csp].sp = cpuvx->sp;
		cpuvx->callstack[cpuvx->csp].fp = cpuvx->fp;
		cpuvx->callstack[cpuvx->csp].ra = cpuvx->pc;
	  cpuvx->stackSize=STACK_SIZE;
	  cpuvx->callstackSize=CALLSTACK_SIZE;
	  cpuvx->br_tableSize=BR_TABLE_SIZE;
	return 1;


}

/*name:vcpuvars_freeContent
description:free a vcpuvars's content arrays
input :VCPUVARS *cpuv
output :NULL
date:20200128*/
void vcpuvars_freeContent(VCPUVARS *cpuv)
{
	free(cpuv->br_table);
	free(cpuv->callstack);
	free(cpuv->stack);
	cpuv->br_tableSize=0;
	cpuv->callstackSize=0;
	cpuv->stackSize=0;
}
/*name:vthread_init
description:initialize a vthread
input :VTHREAD *vthread,uint8_t slevel,uint32_t ticks
output :NULL
date:20200121*/
void vthread_init(VTHREAD *vthread,uint8_t slevel,uint32_t ticks)
{
vthread->st=THREAD_ST_IDLE;
vthread->cpuTicks=ticks;
vthread->level=slevel;
vthread->vlevelP=0;
vthread->localID=(uint32_t)vthread;
list_init(&(vthread->levelList));
list_init(&(vthread->wakeupList));
}
/*name:vthread_new
description:creat new, adnd initialize a vthread
input :uint8_t slevel,uint32_t ticks
output :VTHREAD *
date:20200121*/

VTHREAD *vthread_new(uint8_t slevel,uint32_t ticks)
{
	VTHREAD *vthread=(VTHREAD *)malloc(sizeof(VTHREAD));
	if(vthread==0)
		{return 0;}
	vthread_init(vthread,slevel,ticks);
	return vthread;
}
/*name:vthread_setIdle
description:set a vthread to idle, then update the vtlevel and vtmr it belongs to
input :VTHREAD *vthread
output :NULL
date:20200121*/

void vthread_setIdle(VTHREAD *vthread)
{  VTLEVEL *pLevel;
	VTMR *pMr;
	if(vthread->vlevelP==0)
		{return;}
	if(vthread->st==THREAD_ST_IDLE)
		{return;}
	//change previous level
	pLevel=vthread->vlevelP;
	pMr=pLevel->parent;
	list_del(&(vthread->levelList));
	pLevel->threadCount--;
	//add to idle level
	vthread->vlevelP=&(pMr->idleLevel);
	pLevel=vthread->vlevelP;
	list_add(&(vthread->levelList),&(pLevel->threadList));
	pLevel->threadCount++;
	vtmr_checkUpdateLevel(pMr);
}
/*name:vthread_setActive
description:set a vthread to active, then update the vtlevel and vtmr it belongs to
input :VTHREAD *vthread
output :NULL
date:20200121*/

void vthread_setActive(VTHREAD *vthread)
{
	VTLEVEL *pLevel;
	VTMR *pMr;
	uint8_t myLevel=vthread->level;
	if(vthread->vlevelP==0)
		{return;}
	if(vthread->st==THREAD_ST_RUNNING||vthread->st==THREAD_ST_ACTIVE)
		{return;}
	//change previous level
	pLevel=vthread->vlevelP;
	pMr=pLevel->parent;
	list_del(&(vthread->levelList));
	pLevel->threadCount--;
	//add to right level
	vthread->vlevelP=&(pMr->runLevel[myLevel]);
	pLevel=vthread->vlevelP;
	list_add(&(vthread->levelList),&(pLevel->threadList));
	pLevel->threadCount++;
	vtmr_checkUpdateLevel(pMr);
}
/*name:vthread_fillCpuvars
description:fill a vthread's cpuvar with initial set from a "funcname"
input :VTHREAD *vthread,Module *m,char *funcname
output :int8_t
date:20200121*/

int8_t vthread_fillCpuvars(VTHREAD *vthread,Module *m,char *funcname)
{
	VCPUVARS *cpuv=&(vthread->cpuv);
	return vcpuvars_newFromFunction(cpuv,m,funcname);

}


/*name:vtlevel_init
description:initialize a vtlevel
input :VTLEVEL *vtl
output :NULL
date:20200121*/

void vtlevel_init(VTLEVEL *vtl)
{
	list_init(&(vtl->threadList));
	vtl->threadCount=0;
}
/*name:vtlevel_new
description:creat new and initialize a vtlevel
input :NULL
output :VTLEVEL *
date:20200121*/
VTLEVEL *vtlevel_new(void)
{

	VTLEVEL *vtl=(VTLEVEL *)malloc(sizeof(VTLEVEL));
	if(vtl==0)
	{
		return 0;

	}
	vtlevel_init(vtl);
	return vtl;
}
/*name:vtMR_init
description:initialize a vtmr
input :VTMR *vtmr,uint8_t allLevel
output :int8_t 1 for success
date:20200121*/

int8_t vtmr_init(VTMR *vtmr,uint8_t allLevel)
{
		uint8_t i;
	vtmr->allLevels=allLevel;
	VTLEVEL *pLevel;
	//allocate the runLevel array
	pLevel=(VTLEVEL *)malloc(sizeof(VTLEVEL)*allLevel);
	if(pLevel==0)
		{return 0;}
	vtmr->runLevel=pLevel; 
	//init runLevel array
	for(i=0;i<allLevel;i++)
		{
			vtlevel_init(&(vtmr->runLevel[i]));
			vtmr->runLevel[i].parent=vtmr;
		}
	//init other levels
		vtlevel_init(&(vtmr->idleLevel));
		vtmr->idleLevel.parent=vtmr;
		vtlevel_init(&(vtmr->waitLevel));
		vtmr->waitLevel.parent=vtmr;
	//set runNo to least priority
		vtmr->runNo=allLevel-1;
	//nothing is runnning or will run
		vtmr->nowThread=0;
		vtmr->nextThread=0;
	//
		vtmr->tickCount=0;
	return 1;
}
/*name:vtmr_new
description:ccreat new and initialize a vtmr
input :VTMR *vtmr,uint8_t allLevel
output :VTMR *
date:20200121*/
VTMR *vtmr_new(uint8_t allLevel)
{
	VTMR *vtmr=(VTMR *)malloc(sizeof(VTMR));
	if(vtmr==0)
		{return 0;}
	if(vtmr_init(vtmr,allLevel)==0)
		{return 0;}
	return vtmr;

}
/*name:vtmr_checkUpdateLevel
description:to check if a vtmr's running level should be changed, if so, update everything
input :VTMR *vtmr
output :NULL
note: this should be called if a thread is removed or has status changed
date:20200121*/

void vtmr_checkUpdateLevel(VTMR *vtmr)
{	uint8_t i;
	VTLEVEL *pLevel=&vtmr->runLevel[vtmr->runNo];
	VTHREAD *vthread;
	ELIST *tlist;
	//find the highest priority again
	for(i=0;i<vtmr->allLevels;i++)
		{	
			if(vtmr->runLevel[i].threadCount>0)
				{break;}
		}
	vtmr->runNo=i;
	tlist=vtmr->runLevel[i].threadList.next;//find the 1st thread in new level
	vthread=list_entry(tlist,VTHREAD,levelList);
	vtmr->nextThread=vthread;
	
}
/*name:vtmr_refresh
description:to refresh a vtmr's working status, if needed, run thread switch
input :VTMR *vtmr,Module *m
output :int8_t, 1 for task switch happen
date:20200128*/
int8_t vtmr_refresh(VTMR *vtmr,Module *m)
{	VCPUVARS *now,*switchTo;
	VTLEVEL *levelP;
	VTHREAD *threadP;
	ELIST *tlist;
	if(vtmr->nowThread==vtmr->nextThread)//do nothing in this case
		{
			return 0;
		}
	vtmr->tickCount++;
	if(vtmr->tickCount>=vtmr->nowThread->cpuTicks)//run a switch job;
		{   levelP=&vtmr->runLevel[vtmr->runNo];
			vtmr->tickCount=0;
			//get the right cpu vars
			now=&vtmr->nowThread->cpuv;
			switchTo=&vtmr->nextThread->cpuv;
			vtmr->nowThread->st=THREAD_ST_ACTIVE;
			vtmr->nowThread=vtmr->nextThread;
			if(levelP->threadCount==1)
				{   vtmr->nowThread->st=THREAD_ST_RUNNING;
					return 0;
				}
			
			tlist=vtmr->nextThread->levelList.next;
			if(tlist==&levelP->threadList)//jump the head
				{

					tlist=levelP->threadList.next;
				}
			threadP=list_entry(tlist,VTHREAD,levelList);
			vtmr->nextThread=threadP;
			vtmr->nowThread->st=THREAD_ST_RUNNING;
			
			temp_taskSwitch(now,switchTo,m);
			return 1;
		}
		return 0;

}

/*name:vtmr_refreshNoCnd
description:to refresh a vtmr's working status without condition, and clear its tick
input :VTMR *vtmr,Module *m
output :NULL
date:20200128*/
void vtmr_refreshNoCond(VTMR *vtmr,Module *m)
{	VCPUVARS *now,*switchTo;
	VTLEVEL *levelP;
	VTHREAD *threadP;
	ELIST *tlist;
	if(vtmr->nowThread==vtmr->nextThread)//do nothing in this case
		{
			return;
		}
	//vtmr->tickCount=0;
	  levelP=&vtmr->runLevel[vtmr->runNo];
			vtmr->tickCount=0;
			//get the right cpu vars
			now=&vtmr->nowThread->cpuv;
			switchTo=&vtmr->nextThread->cpuv;
			vtmr->nowThread->st=THREAD_ST_ACTIVE;
			vtmr->nowThread=vtmr->nextThread;
			if(levelP->threadCount==1)
				{   vtmr->nowThread->st=THREAD_ST_RUNNING;
					return;
				}
			
			tlist=vtmr->nextThread->levelList.next;
			if(tlist==&levelP->threadList)//jump the head
				{

					tlist=levelP->threadList.next;
				}
			threadP=list_entry(tlist,VTHREAD,levelList);
			vtmr->nextThread=threadP;
			vtmr->nowThread->st=THREAD_ST_RUNNING;
			temp_taskSwitch(now,switchTo,m);
		//	interpret(m);
}
/*name:vthread_addToMr
description:add a VTHREAD to a VTMR's idle array
input :VTHREAD *vthread,VTMR *vtmr
output :int8_t ,1 for success
date:20200123*/

int8_t vthread_addToMr(VTHREAD *vthread,VTMR *vtmr)
{
	if(vthread==0||vtmr==0)
		{return -1;}
	if(vthread->level>vtmr->allLevels)
		{return 0;}
	list_add(&vthread->levelList,&vtmr->idleLevel.threadList);
	vtmr->idleLevel.threadCount++;
	vthread->vlevelP=&vtmr->idleLevel;
	vthread->st=THREAD_ST_IDLE;
}

/*name:vtmr_getLocalThread
description:find a  thread in vtmr by its localID
input :VTMR *vtmr,uint32_t localId
output :VTHREAAD *
date:20200128*/
VTHREAD *vtmr_getLocalThread(VTMR *vtmr,uint32_t localId)
{
	ELIST *tlist;
	VTHREAD *vthread;
	uint8_t i;
	//looking up in running levels
	for(i=0;i<vtmr->allLevels;i++)
	{
		list_for_each(tlist,&vtmr->runLevel[i].threadList)
		{
			vthread=list_entry(tlist,VTHREAD,levelList);
			if(vthread->localID==localId)
			{	printf("find id %d\r\n",localId);
				return vthread;
			}
		}
	}
	//looking up in idle
	list_for_each(tlist,&vtmr->idleLevel.threadList)
		{
			vthread=list_entry(tlist,VTHREAD,levelList);
			if(vthread->localID==localId)
			{	printf("find id %d\r\n",localId);
				return vthread;
			}
		}
		return 0;
}
/*name:vtmr_endLocalRun
description:end a  thread through its localid
input :uint32_t localId,VTMR *vtmr,Module *m
output :int8_t ,1 for success
date:20200128*/
int8_t vtmr_endLocal(uint32_t localId,VTMR *vtmr,Module *m)
{
	
	VTHREAD *vthread;
	VTHREAD *vnext;
	VTLEVEL *vlevel;
	vthread=vtmr_getLocalThread(vtmr,localId);
	if(vthread==0)
	{return -1;}
	vlevel=vthread->vlevelP;
	//change this level
	vlevel->threadCount--;
	list_del(&vthread->levelList);
	vtmr_checkUpdateLevel(vtmr);
	if(vthread==vtmr->nowThread)
	{
		vtmr_refreshNoCond(vtmr,m);
		//then clear it
		vcpuvars_freeContent(&vthread->cpuv);
		free(vthread);
		interpret(m);//execute!
	}
	return 1;
}

/*name:vcpuvars_binSize
description:to calculate a vcpuvars's bin formate size
input :VCPUVARS *cpuv
output :uint32_t for size in byte
date:20200129*/
uint32_t vcpuvars_binSize(VCPUVARS *cpuv)
{
	uint32_t allSize,headSize;
	headSize=sizeof(VCPUVARS)-sizeof(StackValue *)-sizeof(Frame *)-sizeof(uint32_t   *);
	allSize=headSize+(cpuv->stackSize*sizeof(StackValue))+(cpuv->callstackSize*sizeof(Frame))+(cpuv->br_tableSize*sizeof(uint32_t));
	return allSize;
}
/*name:vcpuvars_toExistBin
description:construct a dataBin(already in memory) from a cpuvar
input :VCPUVARS *cpuv,uint8_t *data
output :NULL
date:20200129*/
void vcpuvars_toExistBin(VCPUVARS *cpuv,uint8_t *data)
{
	uint32_t allsize,i,headsize,offsetStack,offsetCall,offsetBr;
	uint8_t *ptr,*str;
	headsize=sizeof(VCPUVARS)-sizeof(StackValue *)-sizeof(Frame *)-sizeof(uint32_t   *);
	offsetStack=headsize;
	offsetCall=offsetStack+(cpuv->stackSize*sizeof(StackValue));
	offsetBr=offsetCall+(cpuv->callstackSize*sizeof(Frame));
	//pack the head
	ptr=(uint8_t *)cpuv;
	for(i=0;i<headsize;i++)
	{
		data[i]=ptr[i];
	}
	
	//pack the stack array
	ptr=(uint8_t *)cpuv->stack;
	for(i=0;i<(cpuv->stackSize*sizeof(StackValue));i++)
	{
		data[i+offsetStack]=ptr[i];
	}
	//pack the callstack
	ptr=(uint8_t *)cpuv->callstack;
	for(i=0;i<(cpuv->callstackSize*sizeof(Frame));i++)
	{
		data[i+offsetCall]=ptr[i];
	}
	//pack the br_table
	ptr=(uint8_t *)cpuv->br_table;
	for(i=0;i<(cpuv->br_tableSize*sizeof(uint32_t));i++)
	{
		data[i+offsetBr]=ptr[i];
	}
}

/*name:vcpuvars_toNewBin
description:construct a new dataBin from a cpuvar
input :VCPUVARS *cpuv
output :uint8_t *
date:20200129*/
uint8_t *vcpuvars_toNewBin(VCPUVARS *cpuv)
{
	uint32_t dataSize,headSize;
	uint8_t *data;
	dataSize=vcpuvars_binSize(cpuv);
	printf("allocated %d \r\n",dataSize);
	data=(uint8_t *)malloc(dataSize);
	if(data==0)
	{return 0;}
	vcpuvars_toExistBin(cpuv,data);
	return data;
}

/*name:vcpuvars_newFromBin
description:construct a vcpuvars from bindata
input :VCPUVARS *cpuv,uint8_t *data,uint32_t dataSize
output :int8_t,1 for success
date:20200130*/

int8_t vcpuvars_newFromBin(VCPUVARS *cpuv,uint8_t *data,uint32_t dataSize)
{
	uint32_t allSize,headSize;
	uint32_t i;
	
	
	StackValue  *stack; // main operand stack,            // callstack pointer
    Frame       *callstack; // callstack
    uint32_t    *br_table; // br_table branch indexes
	uint32_t offsetStack,offsetCall,offsetBr;
	uint8_t *ptr;
	VCPUVARS *tv=(VCPUVARS *)data;
	//first, check size
	headSize=sizeof(VCPUVARS)-sizeof(StackValue *)-sizeof(Frame *)-sizeof(uint32_t   *);
	allSize=vcpuvars_binSize(tv);
	if(allSize!=dataSize)
	{//printf("size error all=%d,but size =%d\r\n",allSize,dataSize);
	return -1;}
	//prepare new arrays
	stack=(StackValue *)malloc(tv->stackSize*sizeof(StackValue));
	callstack=(Frame *)malloc(tv->callstackSize*sizeof(Frame));
	br_table=(uint32_t *)malloc(tv->br_tableSize*sizeof(uint32_t));
	if(stack==0||callstack==0||br_table==0)
	{	
		//printf("alloc error \r\n");
		return -1;
	}
	offsetStack=headSize;
	offsetCall=offsetStack+(tv->stackSize*sizeof(StackValue));
	offsetBr=offsetCall+(tv->callstackSize*sizeof(Frame));
	//memory copy, first head
	// head
	cpuv->csp=tv->csp;
	cpuv->fp=tv->fp;
	cpuv->sp=tv->sp;
	cpuv->pc=tv->pc;
	cpuv->stackSize=tv->stackSize;
	cpuv->br_tableSize=tv->br_tableSize;
	cpuv->callstackSize=tv->callstackSize;
	cpuv->stack=stack;
	cpuv->callstack=callstack;
	cpuv->br_table=br_table;
	cpuv->bStart=tv->bStart;//attention here!!!
	//pack the stack array
	ptr=(uint8_t *)stack;
	for(i=0;i<(tv->stackSize*sizeof(StackValue));i++)
	{
		ptr[i]=data[i+offsetStack];
	}
	//pack the callstack
	ptr=(uint8_t *)callstack;
	for(i=0;i<(tv->callstackSize*sizeof(Frame));i++)
	{
		ptr[i]=data[i+offsetCall];
	}
	//pack the br_table
	ptr=(uint8_t *)br_table;
	for(i=0;i<(tv->br_tableSize*sizeof(uint32_t));i++)
	{
		ptr[i]=data[i+offsetBr];
	}
	return 1;
}

/*name:vcpuvars_fixBlockIndex
description:fix a vcpuvars's block index according to local host
input :VCPUVARS *cpuv,Module *m
output :NULL
date:20200130*/
void vcpuvars_fixBlockIndex(VCPUVARS *cpuv,Module *m)
{	uint32_t i;
	Block *tb;
	if(cpuv->bStart==m->localBlockStart)
	{return;}
	printf("Base in cpv is %d,but local is %d\r\n",cpuv->bStart,m->localBlockStart);
	for(i=0;i<cpuv->callstackSize;i++)
	{tb=cpuv->callstack[i].block;
		cpuv->callstack[i].block=tb-cpuv->bStart+m->localBlockStart;
	}

}