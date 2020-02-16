#include "oscall.h"
//VCPUVARS cpuv0,cpuv1;
uint32_t g_taskRunNo=0;
//extern VTMR g_vtmr;
int8_t temp_taskMr_refresh(Module *m)
{
  /*
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
   */
   
		vtmr_refresh(m->vtmrP,m);

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
uint32_t os_CpuNewThread(Module *m,void *p1,void *p2)
{
	 /* int fidx = 0, res = 0;
	    Block  *func ;
    Type   *type;
	char amsg[64];
uint32_t p=(uint32_t)p1;
 strcpy(amsg, (m->memory.bytes + p));
	printf("creat task in %s",amsg);
  return vcpuvars_newFromFunction(&cpuv1,m,amsg);*/
  char amsg[64];
	VTHREAD *vthread;
	VTMR *vr=(VTMR *)m->vtmrP;
uint32_t p=(uint32_t)p1;
uint32_t ticks=(uint32_t)p2;
 strcpy(amsg, (m->memory.bytes + p));
	printf("creat task in %s",amsg);
	if(vr->nowThread==0)
		{
			vthread=vthread_new(3,2);
			vthread_addToMr(vthread,vr);
			vthread_setActive(vthread);
			vr->nowThread=vthread;
		}
 // return vcpuvars_newFromFunction(&cpuv1,m,amsg);
  vthread=vthread_new(3,ticks);
  vthread_fillCpuvars(vthread,m,amsg);
  vthread_addToMr(vthread,vr);
  vthread_setActive(vthread);
  return vthread->localID;
//  g_vtmr.nextThread=vthread;
	//interpret(m);
//	vcpuvars_getFromM(&cpuv0,m);
}
uint32_t os_terminate(Module *m,void *p1)
{
	 
	//interpret(m);
	//vcpuvars_putToM(&cpuv0,m);
	int i;
	//m->isRun=0;
	uint32_t id=(uint32_t)p1;
	VTMR *vr=(VTMR *)m->vtmrP;
	printf("teminate id=%d\r\n",id);
	
	//m->isRun=1;
	return vtmr_endLocal(id,vr,m);
}
uint32_t os_getRunId(Module *m)
{	VTMR *vr=(VTMR *)m->vtmrP;
	return vr->nowThread->localID;
}
void os_sleep(void *p1)
{
	uint32_t p=(uint32_t)p1;
	sleep(p);

}

//This is a system api call, requiring 3 params, returning a 32-bit 
uint32_t os_apiP3R32(uint32_t idx,void *p1,void *p2,Module *m)
{
switch (idx){
	case 0:os_apiIdx0(p1,m);break;
	case 1:os_apiIdx1(p1);break;
	case IDX_API_NEWTHREAD:return (os_CpuNewThread(m,p1,p2));
	case IDX_API_DELTHREAD:return (os_terminate(m,p1));
    case IDX_ADI_GETRUNID:return (os_getRunId(m));break;






}
return 0;
}

