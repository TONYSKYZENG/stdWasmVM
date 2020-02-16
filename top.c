#include "wasmvm.h"
#include <pthread.h>
#include "vapor.h"
#include <string.h>

#ifndef VM_NOMAIN
int g_threadCnt=0;
unsigned char *test_wasm;
int8_t *g_recvFuncName;
uint32_t g_codeLen;
uint32_t msCount=0;
VTIMERCTL g_vtimerCtl;
NETSERVER g_netServer;
NETSOCKET g_clientSock;
VCPUVARS *g_recvCpuP;
uint64_t g_recvCmd=0;
char *g_timerMsg="hello world";
int8_t g_mode;
int8_t g_clientRecv=0;
int8_t g_clientRun=0;
int8_t g_clientClone=0;

Module *m;
VTMR g_vtmr;
uint8_t g_serverAck=0;
void serverWaitAck(void)
{
	g_serverAck=1;
	while(g_serverAck==1);
}
pthread_t g_clientMt;

void *thread_function(void *t);
void *clone_run(void *t);
void vosRemoteCmd(NETSERVER *netserver,	uint64_t vcmd);

int32_t netsocket_userConnect(NETSOCKET *netsocket)
{	//printf("connected \r\n");
    if(g_mode!='s')
	{
			UNITNODE node1;
		 unitnode_init(&node1,NODE_TYPE_GENERAL,"X86PLATFORM","192.168.0.100","192.168.0.103",2600,8217,8215);
		 vapor_reportUnitnode(netsocket,&node1);
	}
    return 1;
}
int32_t netsocket_userGet(NETSOCKET *netsocket)
{   pthread_t a_thread,t1t,t2t;
	UNITNODE *node1;
	 if(g_mode=='s')
   { if(isVapor(netsocket->recv,netsocket->recvLen))
        {printf("this is a vapor\r\n");
            switch (getVaporType(netsocket->recv,netsocket->recvLen))
            {
            case VAPOR_TYPE_VNAME:
                 printf("VNAME=%s\r\n",vapor_getVname(netsocket->recv,netsocket->recvLen)); /* code */
                 vapor_sendVack(netsocket);
                break;
            case VAPOR_TYPE_ACK:
                printf("this is VAPOR_ACK\r\n"); /* code */
				 g_serverAck=0;
                break;
			case VAPOR_TYPE_UNITNODE:
				printf("this is UNITNODE\r\n");
				node1=vapor_getUnitnode(netsocket->recv,netsocket->recvLen);
				node1->inPort=netsocket->socketPort;
				//unitnode_print(node1);
				netsocket->extData=(uint8_t *)node1;
				netsocket->dataLen=sizeof(UNITNODE);
				node1->extData=(uint8_t *)netsocket;
				//bind each other
				unitnode_print((UNITNODE *)netsocket->extData);
				break;
            default:
                printf("get %s in port %d, total %d bytes\r\n",netsocket->recv,netsocket->socketPort,netsocket->recvLen);
                break;
            }
        
        }
   }
	else
	{ 	uint64_t cmd;
		uint32_t memSize;
		if(isVapor(netsocket->recv,netsocket->recvLen))
        {printf("this is a vapor\r\n");
            switch (getVaporType(netsocket->recv,netsocket->recvLen))
            {
            case VAPOR_TYPE_VCODE:
               test_wasm=vapor_getVcode(netsocket->recv,netsocket->recvLen,&g_codeLen);
			   g_clientRecv=1;
			   printf("receive vcode %d bytes\r\n",g_codeLen);
			   	/* */
			     m = vm_create(test_wasm, g_codeLen);
			  	vapor_sendVack(netsocket);
			   	// 
                break;
			case VAPOR_TYPE_CPUVARS:
				//printf("receive a cpuvar\r\n");
				
				g_recvCpuP=vapor_getVcpuvars(netsocket->recv,netsocket->recvLen);
				
				/*m->isRun=0;
				vm_waitSafe(m);*/
				vcpuvars_fixBlockIndex(g_recvCpuP,m);
				vcpuvars_putToM(g_recvCpuP,m);
				
				m->mtaskEn=0;
				//pthread_cancel(g_clientMt);
				//interpret(m);
				g_clientClone=1;
				printf("load succeed, stacksize=%d\r\n",g_recvCpuP->stackSize);
				vapor_sendVack(netsocket);
				break;
            case VAPOR_TYPE_ACK:
                 printf("this is VAPOR_ACK\r\n"); /* code */
                break;
			case VAPOR_TYPE_RUNMEM:
				  /*m->isRun=0;
				  vm_waitSafe(m);*/
				  printf("memory changed \r\n");
				  free(m->memory.bytes);
				  m->memory.bytes=vapor_getRunMem(netsocket->recv,netsocket->recvLen,&memSize);
				  printf("received %d bytes \r\n",memSize);
				  //m->shouldRestart=1;
				  vapor_sendVack(netsocket);
				  break;

			case VAPOR_TYPE_FUNCNAME:
				g_recvFuncName=vapor_getFuncName(netsocket->recv,netsocket->recvLen);
				printf("recv funcame=%s\r\n",g_recvFuncName);
				vapor_sendVack(netsocket);
				break;
			
			case VAPOR_TYPE_COMMAND:
				cmd=vapor_getVCommand(netsocket->recv,netsocket->recvLen);
                // printf("this is VAPOR_COMMND %d\r\n",cmd); /* code */
				 g_recvCmd=cmd;
				 switch (g_recvCmd)
				{
					case VAPOR_CMD_LOADMODULE:
						//clientLoad();
						//g_clientRecv=1;
						
					break;
					case VAPOR_CMD_RUNFUNC:
						printf("run function\r\n");
						g_clientRun=1;
				 			break;
					case VAPOR_CMD_STOP:
						printf("stop\r\n");
						m->isRun=0;
						vm_waitSafe(m);
						
					break;
					case VAPOR_CMD_CONTINUE:

				   			m->isRun=1;
				   			printf("continue\r\n");
				  		 break;
					case VAPOR_CMD_NOMT:
							m->isRun=0;
							printf("disable multitask\r\n");
							m->mtaskEn=0;
						break;
					case VAPOR_CMD_ENMT:
							m->isRun=0;
							printf("enable multitask\r\n");
							m->mtaskEn=1;
						break;
					default:
					break;
				}
				vapor_sendVack(netsocket);
                break;
            default:
                printf("get %s in port %d, total %d bytes\r\n",netsocket->recv,netsocket->socketPort,netsocket->recvLen);
                break;
            }
		}
		/* code */
	}
	
    return 1;
}
int32_t netsocket_userDisConnect(NETSOCKET *netsocket)
{
    return 1;
}
void netserver_printDetails(NETSERVER *netserver)
{   ELIST *tlist;
    NETSOCKET *cSock;
  //  netserver->sockCount=0;
    printf("\r\n%d sockets in total \r\n",netserver->sockCount);
    printf("they are as follows:\r\n");
    uint32_t i=0;
    list_for_each(tlist,&netserver->clientSock)
        {
            cSock=list_entry(tlist,NETSOCKET,sockList);
            printf("\r\nsock%d, ip=%s,port=%d\r\n",i,cSock->socketIpv4,cSock->socketPort);
            i++;
        }
    
}
void netserver_sendToSockPort(NETSERVER *netserver)
{ uint16_t port;
    int8_t msg[64];
    ELIST *tlist;
    NETSOCKET *cSock;
    printf("enter a port number\r\n");
    scanf("%d",&port);
    printf("enter your message \r\n");
    scanf("%s",msg);
     list_for_each(tlist,&netserver->clientSock)
        {
            cSock=list_entry(tlist,NETSOCKET,sockList);
            if(cSock->socketPort==port)
            {
                netsocket_send(cSock,msg,strlen(msg));
                printf("done\r\n");
                return;
            }
        }
     printf("port %d not found\r\n",port);
}

void *thread_function(void *t)
{   char *str=(char *)t;
	vm_exec(m, str);
	//printf("hello world");
	free_memory();
		free(test_wasm);

}
void *clone_run(void *t)
{	bool result=true;
	 while(result==true)
   {result = interpret(m);}


}
void vosUpload(NETSERVER *netserver)
{
	 uint16_t port;
    int8_t msg[64];
    ELIST *tlist;
    NETSOCKET *cSock;
    printf("enter a port number\r\n");
    scanf("%d",&port);
   
     list_for_each(tlist,&netserver->clientSock)
        {
            cSock=list_entry(tlist,NETSOCKET,sockList);
            if(cSock->socketPort==port)
            {
              vapor_transVcode(cSock,test_wasm,g_codeLen);
			  serverWaitAck();
			   printf("code trans done\r\n");
			 vapor_transFuncName(cSock,"main",strlen("main"));
			  serverWaitAck();
			 printf("functitle trans done\r\n");
			 vapor_sendVCommand(cSock,VAPOR_CMD_LOADMODULE);
			 serverWaitAck();
                printf("load command trans done\r\n");
                return;
            }
        }
     printf("port %d not found\r\n",port);
}
void vosClone(NETSERVER *netserver)
{
	uint16_t port;
    int8_t msg[64];
    ELIST *tlist;
    NETSOCKET *cSock;
    printf("enter a port number\r\n");
    scanf("%d",&port);
   
     list_for_each(tlist,&netserver->clientSock)
        {
            cSock=list_entry(tlist,NETSOCKET,sockList);
            if(cSock->socketPort==port)
            { //first, stop the source cpu
				VCPUVARS tcpuv;
			  m->isRun=0;
			  vm_waitSafe(m);
			  printf("vm has stopped\r\n");
			  vcpuvars_getFromM(&tcpuv,m);
			 //then, stop the remote cpu
				vapor_sendVCommand(cSock,VAPOR_CMD_STOP);
				serverWaitAck();
			 
			//next, trans the changed memory
				vapor_transRunMem(cSock,m->memory.bytes,m->memory.pages*PAGE_SIZE);
				 serverWaitAck();
				 //finally, trans the saved cpuvars
			 	vapor_transVcpuvars(cSock,&tcpuv);
				 serverWaitAck();
                return;
            }
        }
     printf("port %d not found\r\n",port);
}
void vosRemoteCmd(NETSERVER *netserver,	uint64_t vcmd)
{
	 uint16_t port;
    int8_t msg[64];
    ELIST *tlist;
    NETSOCKET *cSock;

    printf("enter a port number\r\n");
    scanf("%d",&port);
    
     list_for_each(tlist,&netserver->clientSock)
        {
            cSock=list_entry(tlist,NETSOCKET,sockList);
            if(cSock->socketPort==port)
            {
             // vapor_transVcode(cSock,test_wasm,g_codeLen);
			  vapor_sendVCommand(cSock,vcmd);
                printf("command trans done\r\n");
                return;
            }
        }
     printf("port %d not found\r\n",port);
}
void vosMenu(int8_t *cmd)
{ if(g_mode=='s')
	{if(strcmp(cmd,"s")==0)
			{
					m->isRun=0;
					vm_waitSafe(m);
					printf("vm has stopped\r\n");
					printf("memory size=%d bytes \r\n",m->memory.pages*PAGE_SIZE);
					//netserver_printDetails(&g_netServer);
			}
			else if(strcmp(cmd,"stack")==0)
			{		VCPUVARS v1,v2;
					uint8_t *td;
					uint32_t dsize;
					m->isRun=0;
					vm_waitSafe(m);
					vcpuvars_getFromM(&v1,m);
					dsize=vcpuvars_binSize(&v1);
					td=vcpuvars_toNewBin(&v1);
					vcpuvars_newFromBin(&v2,td,dsize);
					vcpuvars_putToM(&v2,m);
					free(td);
			}
			else if(strcmp(cmd,"r")==0)
			{
					m->isRun=1;
			}
			else if(strcmp(cmd,"exit")==0)
			{
			
				exit(0);
			}
			else if(strcmp(cmd,"ls")==0)
			{
				m->isRun=0;
					netserver_printDetails(&g_netServer);
			}
			else if(strcmp(cmd,"up")==0)
			{
				m->isRun=0;
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				vosUpload(&g_netServer);
			}
			else if(strcmp(cmd,"rfunc")==0)
			{
				m->isRun=0;
				printf("prepare a remote start\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				//vosUpload(&g_netServer);
				vosRemoteCmd(&g_netServer,VAPOR_CMD_RUNFUNC);
			}
			else if(strcmp(cmd,"rrun")==0)
			{
				m->isRun=0;
				printf("prepare a remote run\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				//vosUpload(&g_netServer);
				vosRemoteCmd(&g_netServer,VAPOR_CMD_CONTINUE);
				serverWaitAck();
			}
			else if(strcmp(cmd,"rstop")==0)
			{
				m->isRun=0;
				printf("prepare a remote stop\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				//vosUpload(&g_netServer);
				vosRemoteCmd(&g_netServer,VAPOR_CMD_STOP);
				serverWaitAck();
			}
			else if(strcmp(cmd,"rsingle")==0)
			{
				m->isRun=0;
				printf("prepare a remote stop\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				//vosUpload(&g_netServer);
				vosRemoteCmd(&g_netServer,VAPOR_CMD_NOMT);
				serverWaitAck();
			}else if(strcmp(cmd,"rmulti")==0)
			{
				m->isRun=0;
				printf("prepare a remote stop\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				//vosUpload(&g_netServer);
				vosRemoteCmd(&g_netServer,VAPOR_CMD_ENMT);
				serverWaitAck();
			}
			else if(strcmp(cmd,"rclone")==0)
			{
				m->isRun=0;
				printf("prepare a cpu clone\r\n");
				printf("now we have:\r\n");
				netserver_printDetails(&g_netServer);
				vosClone(&g_netServer);
			}
			

	}
	else
	{
		/* code */
	}
	
}
uint8_t *loadByteFile(int8_t *fname,int *lenP)
{
	FILE *fp = fopen(fname,"rb");
	uint8_t *readB;
	int len;
	if(fp==0)
		{
			printf("open file error");
			return -1;

		}
	 fseek(fp, 0, SEEK_END);
    len=ftell(fp);
    printf("open %s,size=%d \r\n",fname,len);
	fseek(fp,0L,SEEK_SET);
	readB=(unsigned char *)malloc(len+1);
	fread(readB,len,1,fp);
	fclose(fp);
	*lenP=len;
	return readB;
}
int smain(int argc,char *argv[])
{
char *p;
    int len;
	uint16_t port;
	 pthread_t a_thread,t1t,t2t;
	 int res;
	 char *fname,*funcname;
    init_table();
	//network
	 if(argc < 4)
    {
        printf("eg: ./app IP Port mode\n");
        exit(1);
    }
    port= atoi(argv[2]);
		g_mode=argv[3][0];
    netserver_init(&g_netServer,port,argv[1]);
    netserver_establishV4(&g_netServer);

	if(vtmr_init(&g_vtmr,THREAD_MAXLEVEL)!=1)
		{
			printf("error occur in initializing\r\n");
			return -1;

		}
    //len = sizeof(test_wasm)/sizeof(char);

	fname="hi.wasm";
	funcname="main";
	test_wasm=loadByteFile(fname,&g_codeLen);
    m = vm_create(test_wasm, g_codeLen);
	res = pthread_create(&a_thread, NULL, thread_function, (void *)funcname);
	m->vtmrP=(void*)&g_vtmr;
	if(res != 0)
		{ pthread_t a_thread,t1t,t2t;
			perror("Thread creation failed.");
			exit(EXIT_FAILURE);
		}
	pthread_detach(a_thread);
	 char tp[32];
		while(1)
		{

			scanf("%s",tp);
			vosMenu(tp);
		}
    return 0;
}
int cmain(int argc,char *argv[])
{	 int port = atoi(argv[2]);
	
	  char *fname,*funcname;
	  init_table();
	uint8_t ip[20];
	getLocalIpv4("192.168.0",ip);
	if(vtmr_init(&g_vtmr,THREAD_MAXLEVEL)!=1)
		{
			printf("error occur in initializing\r\n");
			return -1;

	    }
		 if(netsocket_connetctToServer(&g_clientSock,port,argv[1]))
         	printf("Connectt server successful!!\n");
   		 else
            return -1;
			
			//printf("received %d\r\n",netsocket_getBlocked(&g_clientSock,temp,2000));
			//vapor_sendVack(&g_clientSock);
    while(g_clientRun==0&&g_clientClone==0);
  m->vtmrP=(void*)&g_vtmr;
	if(g_clientRun)
	{ //m = vm_create(test_wasm, g_codeLen);
	printf("this is run \r\n");
	int res = pthread_create(&g_clientMt, NULL, thread_function, g_recvFuncName);
	if(res != 0)
		{
			perror("Thread creation failed.");
			exit(EXIT_FAILURE);
		}
	}
	else if(g_clientClone)
	{	printf("this is clone \r\n");
		int res = pthread_create(&g_clientMt, NULL, clone_run,0);
	if(res != 0)
		{
			perror("Thread creation failed.");
			exit(EXIT_FAILURE);
		}
	}
	while(1)
	{
	}
	return 1;
}
int main(int argc,char *argv[])
{
	if(argv[3][0]=='s')
    	return smain(argc, argv);
	else
    	return cmain(argc, argv);
}
/*
int main(int argc,char *argv[]) {
   
    char *p;
    int len;
	 pthread_t a_thread,t1t,t2t;
	 int res;
	 char *fname,*funcname;
    init_table();
    len = sizeof(test_wasm)/sizeof(char);
	if(vtmr_init(&g_vtmr,THREAD_MAXLEVEL)!=1)
		{
			printf("error occur in initializing\r\n");
			return -1;

		}
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
	if(fp==0)
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
	res = pthread_create(&a_thread, NULL, thread_function, (void *)funcname);
	if(res != 0)
		{
			perror("Thread creation failed.");
			exit(EXIT_FAILURE);
		}
	pthread_detach(a_thread);
 //vm_exec(m, funcname);
    //
    time_t t;
	 char tp[32];
	 time(&t);
		while(1)
		{

			scanf("%s",tp);
		
		}
    return 0;
}*/



void systimerTopIsr(void)
{
    time_t t;
    char p[32];
    time(&t);
	msCount++;
	//vtimerctl_refresh(&g_vtimerCtl);
	 strftime(p, sizeof(p), "%T", localtime(&t));
	if(msCount==1000)
   { //strftime(p, sizeof(p), "%Y-%m-%d %H:%M:%S", localtime(&t));

      //printf("\r\n@Jalyn debug -----> date: %s \n", p);
	msCount=0;
		}
}
#endif