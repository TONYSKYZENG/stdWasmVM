#include "hosttimer.h"
#define TIMER_FLAGS_ALLOC		1	
#define TIMER_FLAGS_USING		2	
/**/
void rti_add1(RUNTIMEINFO *rti)
{
rti->runtimems++;
	if(rti->runtimems==1000)
	{	rti->runtimems=0;
		rti->runtimes++;
		if(rti->runtimes==0x7fffffff)
		{
			rti->runtimes=0;
			rti->runtimerefresh++;
			if(rti->runtimerefresh==4000000000)
			{
				rti->runtimes=0;
				rti->runtimems=0;
				rti->runtimerefresh=0;
			}
		}
	}
}
void initSystimer(void)//1MS
{
	int ret;
		
		struct sigevent vtimerEvp;
		struct timespec vtimerSpec;
		struct itimerspec vtimerValue;
		timer_t sysTimer;
		vtimerEvp.sigev_value.sival_ptr = &sysTimer;
		/*定时器到期时，会产生一个信号*/
		vtimerEvp.sigev_notify = SIGEV_SIGNAL; 
		vtimerEvp.sigev_signo = SIGUSR1;
		signal(SIGUSR1,systimerTopIsr);
		/*时钟源选CLOCK_MONOTONIC主要是考虑到系统的实时时钟可能会在
		程序运行过程中更改，所以存在一定的不确定性，而CLOCK_MONOTONIC
		则不会，较为稳定*/
		ret = timer_create(CLOCK_MONOTONIC, &vtimerEvp, &sysTimer);
		if( ret )
			perror("timer_create");
		vtimerValue.it_interval.tv_sec = 0;		
		vtimerValue.it_interval.tv_nsec = 1000000;/*period = 1ms*/
		clock_gettime(CLOCK_MONOTONIC, &vtimerSpec);		   
		vtimerValue.it_value.tv_sec = vtimerSpec.tv_sec ;	  
		vtimerValue.it_value.tv_nsec = vtimerSpec.tv_nsec + 1000000;
		ret = timer_settime(sysTimer, CLOCK_MONOTONIC, &vtimerValue, NULL);
		if( ret )
				perror("timer_settime");
}
/*name: vtimerctl_init
description:init the timer control struct
input :VTIMERCTL *
output :NULL
date:20200117*/
void vtimerctl_init(VTIMERCTL *tc)
{	VTIMER *t;
	tc->count = 0;
	uint32_t i;
	for (i = 0; i < MAX_TIMER; i++) {
		tc->timers0[i].flags = 0;
	}
	t = vtimer_alloc(tc);
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0; 
	tc->t0 = t; 
	tc->next = 0xffffffff; 
	tc->rti.runtimems=0;
	tc->rti.runtimes=0;
	tc->rti.runtimerefresh=0;
}

/*name: vtimer_alloc
description:to allocate a vtimer from controller tc
input :VTIMERCTL *tc
output :VTIMER
date:20200117*/

VTIMER *vtimer_alloc(VTIMERCTL *tc)
{	uint32_t i;
	for (i = 0; i < MAX_TIMER; i++) {
			if (tc->timers0[i].flags == 0) {
				tc->timers0[i].flags = TIMER_FLAGS_ALLOC;
				tc->timers0[i].flags2 = 0;
				return &(tc->timers0[i]);
			}
		}
		return 0; 
}
/*name: vtimer_free
description:to free a vtimer from controller tc
input :VTIMERCTL *tc
output :NULL
date:20200117*/

void vtimer_free(VTIMER *timer)
{
timer->flags = 0; 
	return;
}
/*name: vtimer_init
description:to init a vtimer
input :VTIMER *timer, int32_t data,int32_t (*body)(void *),FIFO32 *fifo
output :NULL
note:body is the call back function
date:20200117*/
void vtimer_init(VTIMER *timer, int32_t data,int32_t (*body)(void *),FIFO32 *fifo)
{
	timer->fifo = fifo;
	timer->mybody=body;
	timer->data = data;
	timer->extlen=0;
		return;
}
/*name: vtimer_setTime
description:to set a timeout value for a vtimer,and activate the timer
input :VTIMER *timer, VTIMERCTL *tc,uint32_t timeout
output :NULL
note:body is the call back function
date:20200117*/

void vtimer_setTime(VTIMER *timer, VTIMERCTL *tc,uint32_t timeout)
{
	VTIMER *t, *s;
	timer->timeout = timeout + tc->count;
	timer->flags = TIMER_FLAGS_USING;
	t =tc->t0;
	if (timer->timeout <= t->timeout) {
		tc->t0 = timer;
		timer->next = t; 
		tc->next = timer->timeout;
		return;

	}
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			s->next = timer; 
			timer->next = t; 
			return;
		}
	}
}
/*name: vtimer_cancel
description:to cancel atimer
input :VTIMER *timer,VTIMERCTL *tc
output :int32_t
note:body is the call back function
date:20200117*/
int32_t vtimer_cancel(VTIMER *timer,VTIMERCTL *tc)
{
			VTIMER *t;
		if (timer->flags == TIMER_FLAGS_USING) {
			if (timer == tc->t0) {
				t = timer->next;
				tc->t0 = t;
				tc->next = t->timeout;
			} else {
				t = tc->t0;
				for (;;) {
					if (t->next == timer) {
						break;
					}
					t = t->next;
				}
				t->next = timer->next; 
			}
			timer->flags = TIMER_FLAGS_ALLOC;
		
			return 1;	
		}
		return 0; 
}
/*name: vtimerctl_refresh
description:refresh a timerctl,called by systimerTopIsr
input :VTIMERCTL *tc
output :int32_t
note:body is the call back function
date:20200117*/

int32_t vtimerctl_refresh(VTIMERCTL *tc)
{
VTIMER *timer;
int32_t ru=0;
	void *p;
	//int8_t ts = 0;
	tc->count++;
	rti_add1(&tc->rti);
	if (tc->next > tc->count) {
		return ru;  /*no time out happen*/
	}
	timer = tc->t0; /*sth happen here! */
	for (;;) {
		if (timer->timeout > tc->count) {
			break;
		}
        //find a time out
       p = (void*)timer->extattr;
        ru=timer->mybody(p); //call function
		//timer->flags = TIMER_FLAGS_ALLOC;
		vtimer_cancel(timer,tc);
		timer = timer->next; 
	}
	tc->t0 = timer;
	tc->next = timer->timeout;
	return ru;
}
/*name: vtimer_reloadAndRun
description:set up a callback funtionto a new timer, set its timeout,and start immidiately
input :VTIMERCTL *tc,int32_t (*body)(void *),uint32_t timeout
output :int32_t, -1 for unsuccessful
note:you'd better use it in a vtimer's ISR function,not for the 1st time of its setting-up
this version the fifo32 is not used,but it MAYBE be used in the futrue
date:20200117*/
int32_t vtimer_reloadAndRun(VTIMERCTL *tc,int32_t (*body)(void *),uint32_t timeout,void *p,uint32_t attrlen)
{
VTIMER *s_timer=vtimer_alloc(tc);
if(s_timer==0)
{
return -1;
}
vtimer_init(s_timer,0,body,0);
s_timer->extattr=p;
s_timer->extlen=attrlen;
vtimer_setTime(s_timer,tc,timeout);
return 1;
}
/*name: vtimerctl_getS
description:get the runtime-second since the tc set up
input :VTIMERCTL *tc
output :uint32_t
date:20200117*/
uint32_t vtimerctl_getS(VTIMERCTL *tc)
{
return tc->rti.runtimes;
}
/*name: vtimerctl_getMs
description:get the runtime-milliesecond since the tc set up
input :VTIMERCTL *tc
output :uint32_t
date:20200117*/
uint32_t vtimerctl_getMs(VTIMERCTL *tc)
{
return tc->rti.runtimems;
}
/*name: vtimerctl_getMs
description:get the runtime-refresh time since the tc set up
input :VTIMERCTL *tc
output :uint32_t
date:20200117*/
uint32_t vtimerctl_getR(VTIMERCTL *tc)
{
return tc->rti.runtimerefresh;
}