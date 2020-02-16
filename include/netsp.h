#ifndef _NETSP_H_
#define _NETSP_H_

#include "TDP.h"
#include "netconfig.h"
#include "list.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
int8_t getLocalIpv4(uint8_t * workBase,uint8_t *ip);//must be provideed by driver net
//define of server
typedef struct NETSERVER
{
uint16_t  port; //openport
int32_t lfd;//attr number in a system
int8_t *myIpv4;//like"192.168.1.21"
uint32_t sockCount;//amount of active sockets
ELIST clientSock;
}NETSERVER;
void netserver_init(NETSERVER *ser,uint16_t port,int8_t *ipv4);
int8_t netserver_establishV4(NETSERVER *ser);
void netserver_close(NETSERVER *ser);
typedef struct NETSOCKET
{
uint16_t  socketPort; //openport
int8_t *socketIpv4;//like"192.168.1.21", always refer to the ip of its commnunication-parterner(NOT SELF)
uint32_t idInSys;
uint8_t *extData;
uint32_t dataLen;
uint8_t *recv;
uint32_t recvLen;
NETSERVER *serverP;
ELIST sockList;
//the following vars are used for the switch of blocking and non-blocking mode
uint8_t shouldBlock;
uint8_t isBlock;
uint8_t userAck;
}NETSOCKET;
void netsocket_initForServer(NETSOCKET *netsocket,NETSERVER *sP,uint32_t id,uint16_t s_port,int8_t *ipv4);
void netsocket_waitUsrAck(NETSOCKET *netsocket);
int8_t netsocket_connetctToServer(NETSOCKET *netsocket,uint16_t s_port,int8_t *ipv4);//better defined by low-level
int8_t netsocket_connetctToServerBlocked(NETSOCKET *netsocket,uint16_t s_port,int8_t *ipv4);//better defined by low-level
int8_t netsocket_send(NETSOCKET *netsocket,uint8_t *data,uint32_t size);
void netsocket_setBlocked(NETSOCKET *netsocket);
void netsocket_setUnblocked(NETSOCKET *netsocket);
uint32_t netsocket_getBlocked(NETSOCKET *netsocket,uint8_t *data,uint32_t maxsize);
void netsocket_close(NETSOCKET *netsocket);
//void netsocket_initForServer(NETSOCKET *netsocket,NETSERVER *sP,uint32_t id,uint16_t s_port,int8_t *ipv4);
//these 3 functions must be provided by user, and are working as Isrs
int32_t netsocket_userConnect(NETSOCKET *netsocket);
int32_t netsocket_userGet(NETSOCKET *netsocket);
int32_t netsocket_userDisConnect(NETSOCKET *netsocket);
void netsocket_waitUsrAck(NETSOCKET *netsocket);
void netsocket_clearUsrAck(NETSOCKET *netsocket);
int expServerMain(int argc, const char* argv[]);
int expClientMain(int argc, const char* argv[]);
#ifdef __cplusplus
}
#endif



#endif