#include "timerunix.h"
void initSystimer(void)//1MS
{
	int ret;
		struct sigevent vtimerEvp;
		struct timespec vtimerSpec;
		struct itimerspec vtimerValue;
		timer_t sysTimer;
		vtimerEvp.sigev_value.sival_ptr = &sysTimer;
		/*��ʱ������ʱ�������һ���ź�*/
		vtimerEvp.sigev_notify = SIGEV_SIGNAL; 
		vtimerEvp.sigev_signo = SIGUSR1;
		signal(SIGUSR1,systimerTopIsr);
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
