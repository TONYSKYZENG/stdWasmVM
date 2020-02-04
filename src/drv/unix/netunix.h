#ifndef _NETUNIX_H_
#define _NETUNIX_X_
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <ctype.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>
#include "netsp.h"
#define LOCAL_IP_BASE "192.168.0"
#define NET_BUF_LEN_UNIX 5120
typedef struct SOCKINFO
{
    int32_t m_fd;
    pthread_t m_pthid;
    struct sockaddr_in m_addr;
    NETSOCKET *nsp;//must reserve a pointer to platform-no-concern struct
}SOCKINFO;

#endif