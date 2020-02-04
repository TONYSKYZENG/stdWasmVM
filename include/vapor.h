/*note:vapor defines a set of structrues and functions of
communications betewn each fogs and clouds.
The stage from low to high is :low-netsp->netsp->TDP->vapor,
so vapor is an extension of TDP*/
#ifndef _VAPOR_H_
#define _VAPOR_H_
#include "TDP.h"
#include "netsp.h"
#include "ostask.h"
#include "unitnode.h"
//defines of vapor attr
#define ATTR_VAPOR 71
#define ATTR_VAPOR_LARGE_BEGIN 72
#define ATTR_VAPOR_LARGE_NEXT 73
#define ATTR_VAPOR_LARGE_END 74
#define BIN_BLOCK_SIZE 1024
#define VAPOR_BLOCK_SIZE (BIN_BLOCK_SIZE-(sizeof(TDPPACK)-sizeof(uint8_t *))) 
//defines of vapor type
#define VAPOR_TYPE_VNAME 21
#define VAPOR_TYPE_VCODE 22
#define VAPOR_TYPE_COMMAND 23
#define VAPOR_TYPE_FUNCNAME 24
#define VAPOR_TYPE_CPUVARS 25
#define VAPOR_TYPE_RUNMEM 26
#define VAPOR_TYPE_UNITNODE 27
#define VAPOR_TYPE_ACK 120
//defines of vapor command
#define VAPOR_CMD_NULL 0
#define VAPOR_CMD_LOADMODULE 1
#define VAPOR_CMD_LOADCPUVARS 2
#define VAPOR_CMD_RUNFUNC 3
#define VAPOR_CMD_STOP 4
#define VAPOR_CMD_CONTINUE 5
#define VAPOR_CMD_NOMT 6 //disable multi-thread
#define VAPOR_CMD_ENMT 7 //enable multi-thread
#define VAPOR_CMD_RUNCLONE 8

//functions
//Vname
int8_t vapor_reportVname(NETSOCKET *netsock,int8_t *vname,uint32_t nameSize);
uint8_t *vapor_getVname(uint8_t *rawBin,uint32_t readSize);
//funcname
int8_t vapor_transFuncName(NETSOCKET *netsock,int8_t *fname,uint32_t nameSize);
uint8_t *vapor_getFuncName(uint8_t *rawBin,uint32_t readSize);
//Vcode
int8_t vapor_transVcode(NETSOCKET *netsock,uint8_t *vcode,uint32_t codeSize);
uint8_t *vapor_getVcode(uint8_t *rawBin,uint32_t readSize,uint32_t *codeSize);
//command
int8_t vapor_sendVCommand(NETSOCKET *netsock,uint64_t command);
uint64_t vapor_getVCommand(uint8_t *rawBin,uint32_t readSize);
//cpuvar
int8_t vapor_transVcpuvars(NETSOCKET *netsock,VCPUVARS *cpuv);
VCPUVARS *vapor_getVcpuvars(uint8_t *rawBin,uint32_t readSize);
//run memory
int8_t vapor_transRunMem(NETSOCKET *netsock,uint8_t *data,uint32_t size);
uint8_t *vapor_getRunMem(uint8_t *rawBin,uint32_t readSize,uint32_t *memSize);
//unitnode
int8_t vapor_reportUnitnode(NETSOCKET *netsock,UNITNODE *unode);
UNITNODE* vapor_getUnitnode(uint8_t *rawBin,uint32_t readSize);
//ack
int8_t vapor_sendVack(NETSOCKET *netsock);
//check
int8_t isVapor(uint8_t *rawBin,uint32_t readSize);
uint8_t getVaporType(uint8_t *rawBin,uint32_t readSize);
#endif