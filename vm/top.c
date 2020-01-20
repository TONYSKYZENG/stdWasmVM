#include "wasmvm.h"
#include <pthread.h>
#ifndef VM_NOMAIN
int g_threadCnt=0;
unsigned char *test_wasm;
uint32_t msCount=0;
VTIMERCTL g_vtimerCtl;
char *g_timerMsg="hello world";
Module *m;
int32_t t0Isr(void *p)
{
	char *s=(char *)p;
	printf("%s",s);
	vtimer_reloadAndRun(&g_vtimerCtl,t0Isr,500,"HELLO FROM T0",14);
	return strlen(s);
}
int32_t t1Isr(void *p)
{
	char *s=(char *)p;
	printf("%s",s);
	vtimer_reloadAndRun(&g_vtimerCtl,t1Isr,600,"HELLO FROM T1",14);
	return strlen(s);
}

void *thread_function(void *t)
{   char *str=(char *)t;
	vm_exec(m, str);
	printf("hello world");
	free_memory();
		free(test_wasm);

}

int main(int argc,char *argv[]) {
   
    char *p;
    int len;
	 pthread_t a_thread,t1t,t2t;
	 int res;
	 char *fname,*funcname;
#ifdef VOS_EN
printf("enable vos \r\n");
//initSystimer();
/*vtimerctl_init(&g_vtimerCtl);
vtimer_reloadAndRun(&g_vtimerCtl,t0Isr,500,"HELLO FROM T0",14);
vtimer_reloadAndRun(&g_vtimerCtl,t1Isr,600,"HELLO FROM T1",14);
*/
#endif
    init_table();
    len = sizeof(test_wasm)/sizeof(char);
	if(argc<=2)
		{
		printf("paras error,using default settings");
		fname="hi.wasm";
		funcname="main";
					//return -1;
		}
	else
		{
			fname=argv[1];
			funcname=argv[2];

		}
	FILE *fp = fopen(fname,"rb");
	if(fp==NULL)
		{
			printf("open file error");
			return -1;

		}
	 fseek(fp, 0, SEEK_END);
    len=ftell(fp);
    printf("open %s,size=%d \r\n",fname,len);
	fseek(fp,0L,SEEK_SET);
	test_wasm=(unsigned char *)malloc(len+1);
	fread(test_wasm,len,1,fp);
	fclose(fp);
    m = vm_create(test_wasm, len);
	/*res = pthread_create(&a_thread, NULL, thread_function, (void *)funcname);
	if(res != 0)
		{
			perror("Thread creation failed.");
			exit(EXIT_FAILURE);
		}*/
	//pthread_join(a_thread, NULL);
 vm_exec(m, funcname);
    //
    time_t t;
	 char tp[32];
	 time(&t);
		#ifdef VOS_EN
			while(1)
				{
			sleep(2);
			
				}
		#endif
    return 0;
}
#endif
void systimerTopIsr(void)
{
    time_t t;
    char p[32];
    time(&t);
	msCount++;
	//vtimerctl_refresh(&g_vtimerCtl);
	 strftime(p, sizeof(p), "%T", localtime(&t));
	if(msCount==1000)
   { strftime(p, sizeof(p), "%Y-%m-%d %H:%M:%S", localtime(&t));

      printf("\r\n@Jalyn debug -----> date: %s \n", p);
	msCount=0;
		}
}



