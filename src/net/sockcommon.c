#include "netsp.h"

/*name:netserver_init
description:init a NETSERVER struct, setting its port and ipv4
input:NETSERVER *ser,int16_t port,int8_t *ipv4)
output:NULL
note:this function will not establish the real connection, and it is not platform-concerned
date:20190125
*/
void netserver_init(NETSERVER *ser,uint16_t port,int8_t *ipv4)
{

    ser->myIpv4=ipv4;
    ser->port=port;
    list_init(&ser->clientSock);
    ser->sockCount=0;

}
/*name:netsocket_initForServer
description:init a NETSocket struct for server mode use, setting its port and ipv4, then link it to an existing server
input:NETSOCKET *netsocket,uint16_t s_port,int8_t *ipv4
output:NULL
note:in normal case this function is not used by outside, as id is often assigned by driver
date:20190125
*/
void netsocket_initForServer(NETSOCKET *netsocket,NETSERVER *sP,uint32_t id,uint16_t s_port,int8_t *ipv4)
{
    netsocket->dataLen=0;
    netsocket->extData=0;
    netsocket->socketPort=s_port;
    netsocket->socketIpv4=ipv4;
    netsocket->serverP=sP;
    netsocket->idInSys=id;
    netsocket->recv=0;
    netsocket->recvLen=0;
    list_init(&netsocket->sockList);
    netsocket->serverP=sP;
    list_add(&netsocket->sockList,&sP->clientSock);
    sP->sockCount++;
    netsocket->isBlock=0;
    netsocket->shouldBlock=0;
  //  list_add(&netsocket->sockList,&sP->clientSock);
    //
}

/*name:netsocket_setBlocked
description:set a NETSOCKET to blocked status, and wait until procedure finished
input:NETSOCKET *netsocket
output:NULL
date:20190204
*/
void netsocket_setBlocked(NETSOCKET *netsocket)
{
  netsocket->shouldBlock=1;
  while(netsocket->isBlock==0);

}

/*name:netsocket_setUnblocked
description:set a NETSOCKET to unblocked status
input:NETSOCKET *netsocket
output:NULL
date:20190204
*/
void netsocket_setUnblocked(NETSOCKET *netsocket)
{
  netsocket->shouldBlock=0;
}