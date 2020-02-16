#include "wasmvm.h"
#include <pthread.h>
#include "vapor.h"
#include <string.h>


VTMR g_vtmr;
NETSOCKET g_clientSock;
NETSOCKET g_helperSock;
NETSERVER g_netServer;
UNITNODE g_unode;
char g_ip[20];
uint8_t *test_wasm;
Module *g_mP;
uint8_t *g_funcName;
pthread_t g_clientMt;
uint8_t g_mIsRun=0;
UNITNODE *g_helperNode=0;
void *thread_vm(void *t)
{
	 char *str=(char *)t;
	vm_exec(g_mP, str);
	//printf("hello world");
	free_memory();
		free(test_wasm);
}


void *clone_run(void *t)
{	bool result=true;
	 while(result==true)
   {result = interpret(g_mP);}


}
int32_t netsocket_userConnect(NETSOCKET *netsocket)
{	if(netsocket==&g_clientSock) //client
	{
       //UNITNODE node1;
		printf("tx my node info\r\n");
		int res=vapor_reportUnitnode(netsocket,&g_unode);
		if(res)
			printf("done \r\n");
		else 
		 	printf("failed with%d\r\n",res);
	}
	else if(netsocket==&g_helperSock)
	{
		printf("link to helper\r\n");
	}
	else
	{
		printf("I am about to help somebody \r\n");
	}
	
	
    return 1;
}
void getAsClient(NETSOCKET *netsocket)
{
		if(isVapor(netsocket->recv,netsocket->recvLen))
		{
			if(getVaporType(netsocket->recv,netsocket->recvLen)==VAPOR_TYPE_COMMAND)

			{
				if(vapor_getVCommand(netsocket->recv,netsocket->recvLen)==VAPOR_CMD_RUNFUNC)
				{ 	if(g_mIsRun==0)
					{int res = pthread_create(&g_clientMt, NULL, thread_vm, g_funcName);
					pthread_detach(&g_clientMt);
					g_mIsRun=1;
					}
					vapor_sendVack(&g_clientSock);
				}
				else if(vapor_getVCommand(netsocket->recv,netsocket->recvLen)==VAPOR_CMD_STOP)
					{
						if(g_mIsRun==1)
						{
							g_mP->isRun=0;
							vm_waitSafe(g_mP);
						}
						vapor_sendVack(&g_clientSock);
					}
				else if(vapor_getVCommand(netsocket->recv,netsocket->recvLen)==VAPOR_CMD_CONTINUE)
					{
						if(g_mIsRun==1)
						{
							g_mP->isRun=1;
						}
						vapor_sendVack(&g_clientSock);
					}
				}
			else if(getVaporType(netsocket->recv,netsocket->recvLen)==VAPOR_TYPE_ACK)
			{
					netsocket->userAck=0;
			}
			else
			{
			
					g_helperNode=vapor_getUnitnode(netsocket->recv,netsocket->recvLen);
					if(g_helperNode!=0)
					{
						printf("received a possible node can help me \r\n");
						unitnode_print(g_helperNode);
						if(netsocket_connetctToServer(&g_helperSock,g_helperNode->outPort,g_helperNode->outAddr))
						  {
							  printf("helper linked!");
							  VCPUVARS tcpuv;
							  //trans the local runtime environment to helper
							   vcpuvars_getFromM(&tcpuv,g_mP);
							   vapor_transRunMem(&g_helperSock,g_mP->memory.bytes,g_mP->memory.pages*PAGE_SIZE);
				 				netsocket_waitUsrAck(&g_helperSock);
			 					vapor_transVcpuvars(&g_helperSock,&tcpuv);
								netsocket_waitUsrAck(&g_helperSock);
						  }
					}
			}
			

			}
			
}
void getAsHelper(NETSOCKET *netsocket)
{
	if(getVaporType(netsocket->recv,netsocket->recvLen)==VAPOR_TYPE_ACK)
			{
					netsocket->userAck=0;
			}
	else
	{	uint32_t memSize;
		VCPUVARS *recvCpuP;
		switch (getVaporType(netsocket->recv,netsocket->recvLen))
		{
		case VAPOR_TYPE_RUNMEM:
			
			 printf("memory changed \r\n");
				  free(g_mP->memory.bytes);
			  g_mP->memory.bytes=vapor_getRunMem(netsocket->recv,netsocket->recvLen,&memSize);
				  printf("received %d bytes \r\n",memSize);
				  //m->shouldRestart=1;
				  vapor_sendVack(netsocket);	  
			break;
		case VAPOR_TYPE_CPUVARS:
				//printf("receive a cpuvar\r\n");
				
				recvCpuP=vapor_getVcpuvars(netsocket->recv,netsocket->recvLen);
				
				/*m->isRun=0;
				vm_waitSafe(m);*/
				vcpuvars_fixBlockIndex(recvCpuP,g_mP);
				vcpuvars_putToM(recvCpuP,g_mP);
				
				g_mP->mtaskEn=0;
				//pthread_cancel(g_clientMt);
				//interpret(m);
				
				printf("load succeed, stacksize=%d\r\n",recvCpuP->stackSize);
				vapor_sendVack(netsocket);
				int res = pthread_create(&g_clientMt, NULL, clone_run, 0);
				pthread_detach(&g_clientMt);
				g_mIsRun=1;
				break;
		default:
			break;
		}
	}
	
}
void getFromHelper(NETSOCKET *netsocket)
{
	if(getVaporType(netsocket->recv,netsocket->recvLen)==VAPOR_TYPE_ACK)
			{
					netsocket->userAck=0;
			}
}
int32_t netsocket_userGet(NETSOCKET *netsocket)
{ 
	if(netsocket==&g_clientSock) //client
	{
			getAsClient(netsocket);
	}
	else if(netsocket==&g_helperSock)
	{
		getFromHelper(netsocket);
	}
	else 
	{
		getAsHelper(netsocket);
	}
		 //printf("%s\r\n",g_ip);
	
    return 1;
}

int32_t netsocket_userDisConnect(NETSOCKET *netsocket)
{  
    return 1;
}
void stdDownLoad(void)
{
	uint8_t data[4096];
	uint32_t codeSize;
	uint64_t vcmd;
	init_table();
	netsocket_getBlocked(&g_clientSock,data,4096);
	test_wasm=vapor_getVcode(g_clientSock.recv,g_clientSock.recvLen,&codeSize);
	if(test_wasm==0)
		{printf("error in get code \r\n");return;}
	printf("get code %d bytes\r\n",codeSize);
	
	vapor_sendVack(&g_clientSock); //get code

	netsocket_getBlocked(&g_clientSock,data,4096);
	g_funcName=vapor_getFuncName(g_clientSock.recv,g_clientSock.recvLen);
	if(g_funcName==0)
		{printf("error in get function name \r\n");return;}
	printf("recv funcame=%s\r\n",g_funcName); 
	vapor_sendVack(&g_clientSock);//get func name
	
	netsocket_getBlocked(&g_clientSock,data,4096);
	vcmd=vapor_getVCommand(g_clientSock.recv,g_clientSock.recvLen);
	if(vcmd==0)
		{printf("error in waitting the load module command");return;}
	if(vcmd==VAPOR_CMD_LOADMODULE)
	{
		g_mP = vm_create(test_wasm, codeSize);
		g_mP->vtmrP=(void*)(&g_vtmr);
			
	vapor_sendVack(&g_clientSock);
	}
	netsocket_setUnblocked(&g_clientSock);
/*
	netsocket_getBlocked(&g_clientSock,data,4096);
	vcmd=vapor_getVCommand(g_clientSock.recv,g_clientSock.recvLen);
	if(vcmd==0)
		{printf("error in waitting the run command");return;}
	if(vcmd==VAPOR_CMD_RUNFUNC)
	{
	int res = pthread_create(&g_clientMt, NULL, thread_vm, g_funcName);
	pthread_detach(&g_clientMt);
	vapor_sendVack(&g_clientSock);
	}
	*/
}

void vosMenu(int8_t *cmd)
{ 
		if(strcmp(cmd,"busy")==0)
			{
				vapor_sendWorkLoad(&g_clientSock,1000);
			}
		else if(strcmp(cmd,"start")==0)
			{
				//vapor_sendWorkLoad(&g_clientSock,1000);
			if(g_mIsRun==0)
				{int res = pthread_create(&g_clientMt, NULL, thread_vm, g_funcName);
				pthread_detach(&g_clientMt);
				g_mIsRun=1;
				}
			}
		else if(strcmp(cmd,"help")==0)
		{
			 
			g_mP->isRun=0;
			vm_waitSafe(g_mP);
			vapor_sendVCommand(&g_clientSock,VAPOR_CMD_CALLHELP);
			netsocket_waitUsrAck(&g_clientSock);
		}

}
int main(int argc,char *argv[])
{	 int port = atoi(argv[2]);
	
	  char *fname,*funcname;
	  init_table();
	  uint16_t tryPort;
	  getLocalIpv4("192.168.0",g_ip);
	  printf("my ip=%s\r\n",g_ip);
	  	 netserver_init(&g_netServer,9975,g_ip);
    	tryPort=9975;
		netserver_init(&g_netServer,tryPort,g_ip);
		unitnode_init(&g_unode,NODE_TYPE_GENERAL,"X86PLATFORM","192.168.0.100",g_ip,2600,8217,tryPort);
		while(netserver_establishV4(&g_netServer)!=1)
		{
			tryPort++;
			netserver_init(&g_netServer,tryPort,g_ip);
			unitnode_init(&g_unode,NODE_TYPE_GENERAL,"X86PLATFORM","192.168.0.100",g_ip,2600,8217,tryPort);
		}
	
	printf("open server port at %d\r\n",tryPort);
	
	if(vtmr_init(&g_vtmr,THREAD_MAXLEVEL)!=1)
		{
			printf("error occur in initializing\r\n");
			return -1;

	    }
		 if(netsocket_connetctToServerBlocked(&g_clientSock,port,argv[1]))
         	{printf("Connectt server successful!!\n");}
   		 else
            {return -1;} //set up the client to local center
	
		stdDownLoad();
		char cmd[64];
		while(1)
		{
			scanf("%s",cmd);
			vosMenu(cmd);
		}
		netserver_close(&g_netServer);
		return 1;
		//standard process of wait loading
}

void systimerTopIsr(void)
{
    time_t t;
    char p[32];
    time(&t);
	//msCount++;
	//vtimerctl_refresh(&g_vtimerCtl);
	 strftime(p, sizeof(p), "%T", localtime(&t));
	/*if(msCount==1000)
   { //strftime(p, sizeof(p), "%Y-%m-%d %H:%M:%S", localtime(&t));

      //printf("\r\n@Jalyn debug -----> date: %s \n", p);
	msCount=0;
		}*/
}
