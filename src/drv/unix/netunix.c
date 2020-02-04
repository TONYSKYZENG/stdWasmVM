#include "netsp.h"
#include "netunix.h"
 #include <ifaddrs.h>

#include <netinet/in.h>

#include <string.h>

#include <arpa/inet.h>
/*name:unixsocket_works
description:a thread to support each unix socket
input:void *arg
output:void *
note: never used by outside
*/

void* unixsocket_works(void* arg)
{
    SOCKINFO *c_info = (SOCKINFO*)arg;

    //obtain client info
    int client_port = ntohs(c_info->m_addr.sin_port);
    char client_Ip[64];

    socklen_t buf_len = sizeof(client_Ip);

    inet_ntop(AF_INET, (void*)&c_info->m_addr.sin_addr.s_addr,client_Ip,buf_len);
    
   // printf("\t\t\t\t Ip %s, port %d,connect successful\n",client_Ip,client_port);

    while(1)
    {
        char buf[NET_BUF_LEN_UNIX] = {0};
        c_info->nsp->isBlock=1;
        while(c_info->nsp->shouldBlock==1);
        c_info->nsp->isBlock=0;
        int read_len = read(c_info->m_fd, buf, sizeof(buf));
        if(read_len > 0)
        {
           // buf[read_len+1]='\n';
            //printf("->-> Obtain if of ,Ip %s, port %d, send info: %s,len=%d\n",client_Ip,client_port,buf,read_len);
            //write(c_info->m_fd,buf,strlen(buf));
            c_info->nsp->recv=(uint8_t *)buf;
            c_info->nsp->recvLen=read_len;
            netsocket_userGet(c_info->nsp);
        }
        else if(0 == read_len)
        {
            printf("\t\t\t\t Ip %s, port %d, disconnect\n",client_Ip,client_port);
            netsocket_userDisConnect(c_info->nsp);
            list_del(&c_info->nsp->sockList);
            close(c_info->m_fd);
            c_info->nsp->serverP->sockCount--;
            
            free(c_info->nsp);
            free(c_info);
            break;
        }
        else if(-1 == read_len)
        {
            perror("read error");
            exit(1);
        }
    }

    return 0;
}

/*name:netserver_watcher_unix
description:a thread to check if new connection established
input:NETSERVER *ser
output:void *
note:never used by outside functions
date:20190125
*/
void *netserver_watcher_unix(void *arg)
{
NETSERVER *ser=(NETSERVER *)arg;
while(1)
   {
	   printf("\a Wait accepting...\n");
	   struct sockaddr_in client_add;

	   socklen_t len = sizeof(client_add);

	   int cfd = accept(ser->lfd,(struct sockaddr*)&client_add, &len); 
	   
	   while(-1 == cfd && cfd == EINTR)
	   {
		   cfd = accept(ser->lfd,(struct sockaddr*)&client_add, &len); 
	   }
	   
	   // supply son pthread info, platform-related
	   SOCKINFO* s_info =(SOCKINFO*)malloc(sizeof(SOCKINFO));

	   s_info->m_fd = cfd;
	   s_info->m_addr= client_add;
       SOCKINFO *c_info = s_info;
       //obtain client info
       int client_port = ntohs(c_info->m_addr.sin_port);
       char client_Ip[64];
       socklen_t buf_len = sizeof(client_Ip);
       inet_ntop(AF_INET, (void*)&c_info->m_addr.sin_addr.s_addr,client_Ip,buf_len);
       printf("\t\t\t\t Ip %s, port %d,connect successful\n",client_Ip,client_port);
    
         // supply son pthread info, platform-not-related
       NETSOCKET *nsk=(NETSOCKET *)malloc(sizeof(NETSOCKET));
       netsocket_initForServer(nsk,ser,cfd,client_port,client_Ip);
       s_info->nsp=nsk;
       //netsocket_addToServer(nsk,ser);
       netsocket_userConnect(nsk);
	   int res = pthread_create(&s_info->m_pthid, 0, unixsocket_works, (void*)s_info);
	   if(res == -1)
	   {
		   perror("pthread_creat error");
		   exit(1);
	   }
	   pthread_detach(s_info->m_pthid);

   }

}

/*name:client_socketunix
description:a thread to check if a client socket has something new
input:void *
output:void *
note:never used by outside functions
date:20190125
*/
void *client_socketunix(void *arg)
{
    NETSOCKET *netsocket=(NETSOCKET *)arg;
    uint32_t len;
    netsocket_userConnect(netsocket);
 while(1)
    {
        //printf("Please input string\n>>");
        char buf[NET_BUF_LEN_UNIX]; 
        //ack-style lock
        netsocket->isBlock=1;
        while(netsocket->shouldBlock==1);
        netsocket->isBlock=0;
        //fgets(buf,sizeof(buf),stdin);
        //write(fd, buf, strlen(buf));
        //printf("send buf: %s\n",buf);
        len = read(netsocket->idInSys,buf,sizeof(buf));
        if(len > 0)
        {
            //printf("Recv buf: %s\n",buf);
            netsocket->recv=buf;
            netsocket->recvLen=len;
            netsocket_userGet(netsocket);

        }else if(len == 0)
        {
            printf("Serer disconnect \n");
            netsocket_userDisConnect(netsocket);
            break;
        }
        else if(-1 == len)
        {
            perror("Read errror");
           // exit(1);
        }else
        {
            printf("I no now\n");
        }
    }

}
/*name:netserver_establishV4
description:establish an IPV4 server in specific platform, according to its data
input:NETSERVER *ser
output:int8_t 1 for success
note:the lfd in ser will be changed after this
date:20190125
*/
int8_t netserver_establishV4(NETSERVER *ser)
{
    struct sockaddr_in serv;
    pthread_t m_watcherID;
    printf("establish ipv4 for unix\r\n");
    ser->lfd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == ser->lfd)
    {
        printf("socket error");
       // exit(1);
       return -1;
    }
    serv.sin_port = htons(ser->port);
    serv.sin_family = AF_INET;  
    inet_pton(AF_INET, ser->myIpv4, &serv.sin_addr.s_addr);

    int res = bind(ser->lfd, (struct sockaddr *)&serv, sizeof(serv));
    if(-1 == res)
    {
        printf("bind error");
        return -1;
    }

      res = listen(ser->lfd, 128);
    if(-1 == res)
    {
        printf("listen error");
        //exit(1);
        return -1;
    }
    res = pthread_create(&m_watcherID, 0, netserver_watcher_unix, (void*)ser);
    if(res == -1)
        {
            printf("pthread_creat error");
          //  exit(1);
          return -1;
        }
	pthread_detach(&m_watcherID);
    printf("done\r\n");
    return 1;
}
/*name:netsocket_connetctToServer
description:establish an IPV4 connection to server in specific platform, according to its data
input:NETSOCKET *netsocket,uint16_t s_port,int8_t *ipv4
output:int8_t 1 for success
date:20190125
*/
int8_t netsocket_connetctToServer(NETSOCKET *netsocket,uint16_t s_port,int8_t *ipv4)
{
  uint16_t port = s_port;
     pthread_t m_watcherID;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(-1 == fd)
    {
        perror("socket error");
       // exit(1);
       return -1;
    }

    struct sockaddr_in serv;

    serv.sin_port =  htons(port);
    serv.sin_family = AF_INET;
    inet_pton(AF_INET, ipv4, &serv.sin_addr.s_addr);

    socklen_t len = sizeof(serv);
    int res = connect(fd,(struct sockaddr *) &serv, len);
    if(-1 == res)
    {
        perror("connect error");
        return -1;
    }

   // printf("Connectt server successful!!\n");
    netsocket->serverP=0;
    netsocket->idInSys=fd;
    netsocket->dataLen=0;
    netsocket->extData=0;
    netsocket->socketIpv4=ipv4;
    netsocket->socketPort=s_port;
     netsocket->isBlock=0;
    netsocket->shouldBlock=0;
     res = pthread_create(&m_watcherID, 0, client_socketunix, (void*)netsocket);

    if(res == -1)
        {
            printf("pthread_creat error");
          //  exit(1);
          return -1;
        }
	pthread_detach(&m_watcherID);
       
    //close(fd);
    return 1;



}

/*name:netsocket_send
description:send data through a socket
input:NETSOCKET *netsocket,uint8_t *data,uint32_t size
output:int8_t 1 for success
date:20190125
*/
int8_t netsocket_send(NETSOCKET *netsocket,uint8_t *data,uint32_t size)
{
write(netsocket->idInSys,data,size);
return 1;

}

/*name:netsocket_getBlocked
description:get data through a socket, this is blocked untill something receive
input:NETSOCKET *netsocket,uint8_t *data
output:uint32_t for received data size in bytes
note:after this, use netsocket_setUnblocked()to go back to unblocked receive
date:20190204
*/
uint32_t netsocket_getBlocked(NETSOCKET *netsocket,uint8_t *data,uint32_t maxsize)
{ uint32_t len;
 netsocket_setBlocked(netsocket_setBlocked);
 len=read(netsocket->idInSys, data, maxsize);
//netsocket->shouldBlock=0;
return len;
}

/*name:getLocalIpv4
description:get local ipv4 used by this program
input:uint8_t *ip
output:int8_t,1 for success
date:20190204
*/
int8_t getLocalIpv4(uint8_t *ip){
        struct ifaddrs *ifAddrStruct;
        void *tmpAddrPtr=NULL;
        //char ip[INET_ADDRSTRLEN];
        int n = 0;
        getifaddrs(&ifAddrStruct);
        while (ifAddrStruct != NULL) {
                if (ifAddrStruct->ifa_addr->sa_family==AF_INET) {
                        tmpAddrPtr=&((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
                        inet_ntop(AF_INET, tmpAddrPtr, ip, INET_ADDRSTRLEN);
                       
                        if(strstr(ip,LOCAL_IP_BASE)!=0)
                        {  printf("%s IP Address:%s\n", ifAddrStruct->ifa_name, ip);
                             break;
                        }
                }
                ifAddrStruct=ifAddrStruct->ifa_next;
        }
        //free ifaddrs
        //freeifaddrs(ifAddrStruct);
        return 1;
}

