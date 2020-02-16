
#ifndef _UNITNODE_H_
#define _UNITNODE_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <stdint.h>
#include "list.h"
#define NODE_TYPE_LPEMBEDED 1 //low power embeded node, without unix-like system
#define NODE_TYPE_GENERAL 2 //local generic device, run unix-like system
#define NODE_TYPE_LOCALCENTER 3 //local center
#define NODE_TYPE_ACC 4 //ACCELERATOR, like fpga or GPU
#define NODE_TYPE_CLOUDCENTER 5 //cloud center
typedef struct UNITNODE
{
   //fixed size varibles
     uint16_t headSize; //tdp v1.3 
    uint16_t nameSize;
    uint16_t inAddrSize;
    uint16_t outAddrSize;
    uint8_t unitType;
    uint16_t inPort;
    uint16_t outPort;
    uint16_t cpuLoad;//0 for free
    uint16_t cpuFre;//frequency in Mhz
    uint16_t errorCode;
    uint8_t *extData;
    ELIST ceilList;//for the seek of management using 
    uint8_t *nodeName;
    uint8_t *inAddr;
    uint8_t *outAddr;
}UNITNODE;
void unitnode_init(UNITNODE *node,uint8_t type,uint8_t *name,uint8_t *inA,uint8_t *outA,uint32_t fre,uint16_t inP,uint16_t outP);
void unitnode_print(UNITNODE *node);
uint8_t *unitnode_toNewBin(UNITNODE *unode);
uint32_t unitnode_binSize(UNITNODE *unode);
void unitnode_toExistBin(UNITNODE *unode,uint8_t *data);
int8_t unitnode_newFromBin(UNITNODE *unode,uint8_t *data,uint32_t dataSize);
typedef struct LSNODE //the node attr in local server
{
    ELIST ceilList;
    uint32_t nodeCount;
    uint8_t *extData;
}LSNODE;
void lsnode_init(LSNODE *lsn);
void lsnode_addNode(LSNODE *lsn,UNITNODE *unode);
void lsnode_delNode(LSNODE *lsn,UNITNODE *unode);
UNITNODE *lsnode_findFreeNode(LSNODE *lsn);
#ifdef __cplusplus
}
#endif
#endif