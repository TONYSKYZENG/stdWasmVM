#include "unitnode.h"
#include <string.h>

/*name:unitnode_init
description:init a unitnode
input :UNITNODE *node,uint8_t type,uint8_t *name,uint8_t *inA,uint8_t *outA,uint32_t fre, uint16_t inP, uint16_t outP
output :nulll
date:20200204*/
void unitnode_init(UNITNODE *node,uint8_t type,uint8_t *name,uint8_t *inA,uint8_t *outA,uint32_t fre,uint16_t inP, uint16_t outP)
{	node->extData=0;
	node->unitType=type;
	node->nodeName=name;
	node->nameSize=strlen(name);
	node->inAddr=inA;
	node->inAddrSize=strlen(inA);
	node->inPort=inP;
	node->outAddr=outA;
	node->outAddrSize=strlen(outA);
	node->outPort=outP;
	node->cpuFre=fre;
	node->cpuLoad=0;
	node->errorCode=0;
	list_init(&node->ceilList);
}

/*name:unitnode_print
description:print the infomation of a unitnode
input :UNITNODE *node
output :nulll
date:20200204*/
void unitnode_print(UNITNODE *node)
{
	printf("\r\n unitnode,name=%s,ns=%d,type=%d\r\n",node->nodeName,node->nameSize,node->unitType);
	printf("inAddr=%s, ins=%d,port=%d\r\n",node->inAddr,node->inAddrSize,node->inPort);
	printf("outAddr=%s,outs=%d,out port=%d\r\n",node->outAddr,node->outAddrSize,node->outPort);
	printf("frequency=%d Mhz,load=%d\r\n",node->cpuFre,node->cpuLoad);
}
/*name:unitnode_toNewBin
description:construct a new dataBin from a cpuvar
input :VCPUVARS *cpuv
output :uint8_t *
date:20200203*/
uint8_t *unitnode_toNewBin(UNITNODE *unode)
{
    uint32_t dataSize,headSize;
	uint8_t *data;
	dataSize=unitnode_binSize(unode);
	//printf("allocated %d \r\n",dataSize);
	data=(uint8_t *)malloc(dataSize);
	if(data==0)
	{return 0;}
	unitnode_toExistBin(unode,data);
	return data;

}

/*name:unitnode_binSize
description:to calculate a unitnode_binSize's bin formate size
input :UNITNODE *unode
output :uint32_t for size in byte
date:20200203*/
uint32_t unitnode_binSize(UNITNODE *unode)
{
    uint32_t allSize,headSize;
	headSize=sizeof(UNITNODE)-sizeof(uint8_t *)-sizeof(uint8_t *)-sizeof(uint8_t*);
	allSize=headSize+(unode->nameSize+unode->inAddrSize+unode->outAddrSize)*sizeof(uint8_t);
    return allSize;
}
/*name:unitnode_toExistBin
description:construct a  dataBin(already in memory) from a cpuvar
input :VCPUVARS *cpuv， uint8_t *data
output:NUll
date:20200203*/
void unitnode_toExistBin(UNITNODE *unode,uint8_t *data)
{
    uint32_t allsize,i,headsize,offsetNodeName,offsetIn,offsetOut;
	uint8_t *ptr,*str;
	headsize=sizeof(UNITNODE)-sizeof(uint8_t *)-sizeof(uint8_t *)-sizeof(uint8_t*);
	offsetNodeName=headsize;
	offsetIn=offsetNodeName+(unode->nameSize*sizeof(uint8_t));
	offsetOut=offsetIn+( unode->inAddrSize*sizeof(uint8_t));
    	ptr=(uint8_t *)unode;
	for(i=0;i<headsize;i++)
	{
		data[i]=ptr[i];
	}
	
	//pack the uname
	ptr=(uint8_t *)unode->nodeName;
    
	for(i=0;i<(unode->nameSize*sizeof(uint8_t));i++)
	{
		data[i+offsetNodeName]=ptr[i];
	}
	//pack the in addr
	ptr=(uint8_t *)unode->inAddr;
	for(i=0;i<(unode->inAddrSize*sizeof(uint8_t));i++)
	{
		data[i+offsetIn]=ptr[i];
	}
	//pack the out addr
	ptr=(uint8_t *)unode->outAddr;
	for(i=0;i<(unode->outAddrSize*sizeof(uint8_t));i++)
	{
		data[i+offsetOut]=ptr[i];
	}
}

/*name:unitnode_newFromBin
description:construct a unitnode from bindata
input :UNITNODE *unode,uint8_t *data,uint32_t dataSize
output :int8_t,1 for success
date:20200203*/

int8_t unitnode_newFromBin(UNITNODE *unode,uint8_t *data,uint32_t dataSize)
{
    uint32_t allSize,headSize,offsetNodeName,offsetIn,offsetOut;
	uint32_t i;
    uint8_t *uname,*inaddr,*outaddr;
    uint8_t *ptr;
    headSize=sizeof(UNITNODE)-sizeof(uint8_t *)-sizeof(uint8_t **)-sizeof(uint8_t*);
    UNITNODE *tu;
    tu=(UNITNODE *)data;
    //size check
    allSize=unitnode_binSize(tu);
    if(allSize!=dataSize)
	{//printf("size error all=%d,but size =%d\r\n",allSize,dataSize);
	return -1;}
	offsetNodeName=headSize;
	offsetIn=offsetNodeName+(tu->nameSize*sizeof(uint8_t));
	offsetOut=offsetIn+( tu->inAddrSize*sizeof(uint8_t));
    uname=(uint8_t *)malloc(tu->nameSize*sizeof(uint8_t));
    inaddr=(uint8_t *)malloc(tu->inAddrSize*sizeof(uint8_t));
    outaddr=(uint8_t *)malloc(tu->outAddrSize*sizeof(uint8_t));
	//	printf("%d,%d,%d\r\n",offsetNodeName,offsetIn,offsetOut);
    if(uname==0||inaddr==0||outaddr==0)
    {return -1;}
    //head
    ptr=(uint8_t *)unode;
    for(i=0;i<headSize;i++)
	{
		ptr[i]=data[i];
	}
    //fix value
    list_init(&unode->ceilList);
   
    //uname
    ptr=(uint8_t *)uname;
    
	for(i=0;i<(unode->nameSize*sizeof(uint8_t));i++)
	{
		ptr[i]=data[i+offsetNodeName];
	}
	//repack the in addr
	ptr=(uint8_t *)inaddr;
	for(i=0;i<(unode->inAddrSize*sizeof(uint8_t));i++)
	{
		ptr[i]=data[i+offsetIn];
	}
	//repack the out addr
	ptr=(uint8_t *)outaddr;
	for(i=0;i<(unode->outAddrSize*sizeof(uint8_t));i++)
	{
		ptr[i]=data[i+offsetOut];
	}
	 unode->nodeName=uname;
    unode->inAddr=inaddr;
    unode->outAddr=outaddr;
    return 1;

}