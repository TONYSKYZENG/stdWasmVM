#ifndef _OSTIMER_H_
#define _OSTIMER_H_
/*OS TIMER:establish a 1ms-cycled running timer for vm use*/
#include <stdint.h>
#include "fifo32.h"
#include "osconfig.h"

#ifdef __cplusplus
extern "C"
{
#endif
void initSystimer(void); //this one must be provided by platform_driver
void systimerTopIsr(void);//this one must be defined in user app
 #define MAX_TIMER	VTIMER_AMOUNT	
typedef struct RUNTIMEINFO
{
uint32_t runtimes,runtimems,runtimerefresh;
}RUNTIMEINFO;
typedef struct VTIMER {
	struct VTIMER *next;
	uint32_t timeout;
	int8_t flags, flags2;
     FIFO32 *fifo;
	 int32_t (* mybody)(void *p);//timer_isr
	void *extattr;//extra atributes
	uint32_t extlen;
	int32_t data;

}VTIMER;
typedef struct VTIMERCTL {
	uint32_t count, next;
	VTIMER *t0;
	RUNTIMEINFO rti;
	VTIMER timers0[MAX_TIMER];
}VTIMERCTL ;
void vtimerctl_init(VTIMERCTL *tc);
VTIMER *vtimer_alloc(VTIMERCTL *tc);
void vtimer_free(VTIMER *timer);
void vtimer_init(VTIMER *timer, int32_t data,int32_t (*body)(void *),FIFO32 *fifo);
void vtimer_setTime(VTIMER *timer,VTIMERCTL *tc, uint32_t timeout);
int32_t vtimer_cancel(VTIMER *timer,VTIMERCTL *tc);
int32_t vtimerctl_refresh(VTIMERCTL *tc);

int32_t vtimer_reloadAndRun(VTIMERCTL *tc,int32_t (*body)(void *),uint32_t timeout,void *p,uint32_t attrlen);
uint32_t vtimerctl_getS(VTIMERCTL *tc);
uint32_t vtimerctl_getMs(VTIMERCTL *tc);
uint32_t vtimerctl_getR(VTIMERCTL *tc);
#ifdef __cplusplus
}
#endif
#endif
