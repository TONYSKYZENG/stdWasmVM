
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "netsp.h"
#include "o12864.h"
extern NETSOCKET *g_cfgSockP;
typedef struct SOCKINFO
{
    int32_t m_fd;
    struct sockaddr_in m_addr;
    NETSOCKET *nsp;//must reserve a pointer to platform-no-concern struct
}SOCKINFO;
void esp32ServerRun(void *pvParameters)
{    char *rx_buffer;
    NETSERVER *ser=(NETSERVER *)pvParameters;
     struct sockaddr_in sourceAddr; // Large enough for both IPv4 or IPv6
        uint addrLen = sizeof(sourceAddr);
        int sock = accept(ser->lfd, (struct sockaddr *)&sourceAddr, &addrLen);
       while(-1 == sock && sock == EINTR)
	   {
		   sock = accept(ser->lfd, (struct sockaddr *)&sourceAddr, &addrLen); 
	   }
      //  ESP_LOGI(TAG, "Socket accepted");
    SOCKINFO* s_info =(SOCKINFO*)malloc(sizeof(SOCKINFO));

	   s_info->m_fd = sock;
	   s_info->m_addr= sourceAddr;
       SOCKINFO *c_info = s_info;
       //obtain client info
       int client_port = ntohs(c_info->m_addr.sin_port);
       char client_Ip[64];
       socklen_t buf_len = sizeof(client_Ip);
       inet_ntop(AF_INET, (void*)&c_info->m_addr.sin_addr.s_addr,client_Ip,buf_len);
       printf("\t\t\t\t Ip %s, port %d,connect successful\n",client_Ip,client_port);
    
         // supply son pthread info, platform-not-related
       NETSOCKET *nsk=(NETSOCKET *)malloc(sizeof(NETSOCKET));
       netsocket_initForServer(nsk,ser,sock,client_port,(int8_t *)client_Ip);
       s_info->nsp=nsk;
       rx_buffer=(char *)malloc(1024);
       //TEMPORARY.
       g_cfgSockP=nsk;
       //netsocket_addToServer(nsk,ser);
       netsocket_userConnect(nsk);
     while (1) {
        nsk->isBlock=1;
        while(nsk->shouldBlock==1);
        nsk->isBlock=0;
            int len = recv(sock, rx_buffer, 1024,0);
            // Error occured during receiving
            if (len < 0) {
            // netsocket_userDisConnect(c_info->nsp);
            
                break;
            }
            // Connection closed
            else if (len == 0) {
           netsocket_userDisConnect(c_info->nsp);
            return;
            }
            // Data received
            else {
           
                 c_info->nsp->recv=(uint8_t *)rx_buffer;
                 c_info->nsp->recvLen=len;
                netsocket_userGet(c_info->nsp);
            
            }
        }
}
/*note:
since esp32 has very low memory capacity,
multi socket for a server is not allowed*/

int8_t netserver_establishV4(NETSERVER *ser)
{
 
 char addr_str[128];
    int addr_family;
    int ip_protocol;
      struct sockaddr_in destAddr;
       // destAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        destAddr.sin_family = AF_INET;
        destAddr.sin_port = htons(ser->port);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        inet_pton(AF_INET, (char *)ser->myIpv4, &destAddr.sin_addr.s_addr);
        //inet_ntoa_r(destAddr.sin_addr, addr_str, sizeof(addr_str) - 1);
    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (listen_sock < 0) {
           return -1;
        }
     

        int err = bind(listen_sock, (struct sockaddr *)&destAddr, sizeof(destAddr));
        if (err != 0) {
          return -1;
        }
   
        ser->lfd=listen_sock;
           err = listen(ser->lfd, 1);
      if (err != 0) {
          return -1;
        }
 xTaskCreate(esp32ServerRun, "tcp_server", 4096, (void *)ser, 10, NULL);
    OLED_ShowString(0,28,(uint8_t *)"server up");
       return 1;
}

/*name:netsocket_send
description:send data through a socket
input:NETSOCKET *netsocket,uint8_t *data,uint32_t size
output:int8_t 1 for success
date:20190214
*/
int8_t netsocket_send(NETSOCKET *netsocket,uint8_t *data,uint32_t size)
{
send(netsocket->idInSys,data,size,0);
return 1;

}

/*name:netsocket_getBlocked
description:get data through a socket, this is blocked untill something receive
input:NETSOCKET *netsocket,uint8_t *data
output:uint32_t for received data size in bytes
note:after this, use netsocket_setUnblocked()to go back to unblocked receive,and the socket's data area can be pointed to data
date:20190214
*/
uint32_t netsocket_getBlocked(NETSOCKET *netsocket,uint8_t *data,uint32_t maxsize)
{ uint32_t len;
 netsocket_setBlocked(netsocket);
 len=recv(netsocket->idInSys,data, maxsize, 0);
 netsocket->recv=data;
 netsocket->recvLen=len;
//netsocket->shouldBlock=0;
return len;
}

/*name:netsocket_close
description:close a socket
input:NETSOCKET *netsocket
output:NULL
date:20190214
*/
void netsocket_close(NETSOCKET *netsocket)
{
   // close(netsocket->idInSys);
  shutdown(netsocket->idInSys,0);
}