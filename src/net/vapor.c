#include "vapor.h"
/*name:vapor_transGeneric
description:trans a generic data through vapor
input:NETSOCKET *netsock,int8_t *data,uint32_t size,uint8_t type
output:int8_t 1 for success
note: always used by other inside function
date:20200128
*/
int8_t vapor_transGeneric(NETSOCKET *netsock,uint8_t *data,uint32_t dataSize,uint8_t type)
{
    //first, generate the tdp
    TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    uint32_t allsize,ppsize,headsize;
    uint8_t *tBin;
    TDPPACK_addInfos(m_tdp,type,ATTR_VAPOR,0);
    m_tdp->data=data;
    m_tdp->size=dataSize;
    //then, pack it
    ppsize=m_tdp->size;
	headsize=sizeof(TDPPACK)-sizeof(uint8_t *);
    m_tdp->headSize=headsize;
	allsize=headsize+ppsize;
    tBin=TDPPACK_toNewBin(m_tdp);
    if(tBin==0)
    {
        return -2;
    }
    //next, send 
    netsocket_send(netsock,tBin,allsize);
    //finally, clean rubbish
    free(tBin);

    return 1;
}
/*name:vapor_getGeneric
description:get a generic data through vapor from rawBin,
input:uint8_t *rawBin,uint32_t readSize,uint8_t type
output:uint8_t *for data, uint32_t *for size
note；always used by inside funtions
date:20200126
*/
uint8_t *vapor_getGeneric(uint8_t *rawBin,uint32_t readSize,uint8_t type,uint32_t *dataSize)
{   uint8_t *data;
    if(getVaporType(rawBin,readSize)!=type)
        {return 0;}
    TDPPACK *tp=TDPPACK_fromExiBin(rawBin);
    *dataSize=tp->size;
    data=TDPPACK_freePackNd(tp);
    return data;
}
/*name:vapor_reportVname
description:report a edge's name through vapor, in regard as certain netsocket
input:NETSOCKET *netsock,int8_t *vname,uint32_t size
output:int8_t 1 for success
date:20200128
*/
int8_t vapor_reportVname(NETSOCKET *netsock,int8_t *vname,uint32_t nameSize)
{
    //first, generate the tdp
   /* TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    uint32_t allsize,ppsize,headsize;
    uint8_t *tBin;
    TDPPACK_addInfos(m_tdp,VAPOR_TYPE_VNAME,ATTR_VAPOR,0);
    m_tdp->data=vname;
    m_tdp->size=nameSize;
    //then, pack it
    ppsize=m_tdp->size;
	headsize=sizeof(TDPPACK)-sizeof(uint8_t *);
	allsize=headsize+ppsize;
    tBin=TDPPACK_toNewBin(m_tdp);
    if(tBin==0)
    {
        return -2;
    }
    //next, send 
    netsocket_send(netsock,tBin,allsize);
    //finally, clean rubbish
    free(tBin);*/

    return vapor_transGeneric(netsock,(uint8_t *)vname,nameSize,VAPOR_TYPE_VNAME);
}
/*name:vapor_transFuncName
description:trans the name of a function to be executed
input:NETSOCKET *netsock,int8_t *vname,uint32_t size
output:int8_t 1 for success
date:20200128
*/
int8_t vapor_transFuncName(NETSOCKET *netsock,int8_t *fname,uint32_t nameSize)
{
    return vapor_transGeneric(netsock,(uint8_t *)fname,nameSize,VAPOR_TYPE_FUNCNAME);
}
/*name:vapor_getFuncName
description:get a function name from rawBin,
input:uint8_t *rawBin,uint32_t readSize
output:uint8_t *
date:20200128
*/
uint8_t *vapor_getFuncName(uint8_t *rawBin,uint32_t readSize)
{  uint32_t len;
    return vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_FUNCNAME,&len);

}
/*name:vapor_sendVack
description:send an ack through vapor, in regard as certain netsocket
input:NETSOCKET *netsock
output:int8_t 1 for success
date:20200126
*/
int8_t vapor_sendVack(NETSOCKET *netsock)
{
     //first, generate the tdp
    TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    m_tdp->headSize=sizeof(TDPPACK)-sizeof(uint8_t *);
    uint8_t tBin[sizeof(TDPPACK)];
  
    TDPPACK_addInfos(m_tdp,VAPOR_TYPE_ACK,ATTR_VAPOR,VAPOR_TYPE_ACK);
    m_tdp->data=0;
    m_tdp->size=0;
    //then, pack it
    TDPPACK_toExiBin(m_tdp,tBin);
    //next, send 
    netsocket_send(netsock,tBin,sizeof(TDPPACK));
    return 1;

}
/*name:vapor_getVname
description:get a vapor name from rawBin,
input:uint8_t *rawBin,uint32_t readSize
output:uint8_t *
date:20200128
*/
uint8_t *vapor_getVname(uint8_t *rawBin,uint32_t readSize)
{  uint32_t len;
    return vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_VNAME,&len);
    /* uint8_t *name;
    if(getVaporType(rawBin,readSize)!=VAPOR_TYPE_VNAME)
        {return 0;}
    TDPPACK *tp=TDPPACK_fromExiBin(rawBin);
    name=TDPPACK_freePackNd(tp);
    return name;*/
}
/*name:vapor_transVcode
description:transfer a bytestream of code through vapor
input:NETSOCKET *netsock,int8_t *vcode,uint32_t codeSize
output:int8_t 1 for success
date:20200129
*/
int8_t vapor_transVcode(NETSOCKET *netsock,uint8_t *vcode,uint32_t codeSize)
{
     //first, generate the tdp
    /*TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    uint32_t allsize,ppsize,headsize;
    uint8_t *tBin;
    TDPPACK_addInfos(m_tdp,VAPOR_TYPE_VCODE,ATTR_VAPOR,0);
    m_tdp->data=vcode;
    m_tdp->size=codeSize;
    //then, pack it
    ppsize=m_tdp->size;
	headsize=sizeof(TDPPACK)-sizeof(uint8_t *);
	allsize=headsize+ppsize;
    tBin=TDPPACK_toNewBin(m_tdp);
    if(tBin==0)
    {
        return -2;
    }
    //next, send 
    netsocket_send(netsock,tBin,allsize);
    //finally, clean rubbish
    free(tBin);*/
    return vapor_transGeneric(netsock,vcode,codeSize,VAPOR_TYPE_VCODE);
   
}
/*name:vapor_getVcode
description:get a vapor code from rawBin,
input:uint8_t *rawBin,uint32_t readSize
output:uint8_t * for code, uint32_t for codeSize
date:20200127
*/
uint8_t *vapor_getVcode(uint8_t *rawBin,uint32_t readSize,uint32_t *codeSize)
{
    /*uint8_t *code;
    if(getVaporType(rawBin,readSize)!=VAPOR_TYPE_VCODE)
        {return 0;}
    TDPPACK *tp=TDPPACK_fromExiBin(rawBin);
    *codeSize=tp->size;
    code=TDPPACK_freePackNd(tp);
    return code;*/
    return vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_VCODE,codeSize);
}
/*name:vapor_sendVCommand
description:send a command through vapor, in regard as certain netsocket
input:NETSOCKET *netsock
output:int8_t 1 for success
date:20200126
*/
int8_t vapor_sendVCommand(NETSOCKET *netsock,uint64_t command)
{
     //first, generate the tdp
    TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    uint8_t tBin[sizeof(TDPPACK)];
    m_tdp->headSize=sizeof(TDPPACK)-sizeof(uint8_t *);
    TDPPACK_addInfos(m_tdp,VAPOR_TYPE_COMMAND,ATTR_VAPOR,command);
    m_tdp->data=0;
    m_tdp->size=0;
    //then, pack it
    TDPPACK_toExiBin(m_tdp,tBin);
    //next, send 
    netsocket_send(netsock,tBin,sizeof(TDPPACK));
    return 1;
}
/*name:vapor_getVCommand
description:to get a command from vapor
input:uint8_t *rawBin,uint32_t readSize
output:uint64_t 0 for illegaluint32_t *codeSize
date:20200126*/
uint64_t vapor_getVCommand(uint8_t *rawBin,uint32_t readSize)
{
    uint64_t vcmd;
    if(getVaporType(rawBin,readSize)!=VAPOR_TYPE_COMMAND)
        {return 0;}
    TDPPACK *tp=TDPPACK_fromExiBin(rawBin);
    vcmd=tp->reserved;
    TDPPACK_freePackNd(tp);
    return vcmd;
}
/*name:isVapor
description:to check if a rawBin reports a vapor
input:uint8_t *rawBin,uint32_t readSize
output:int8_t 1 for yes
date:20200126
*/
int8_t isVapor(uint8_t *rawBin,uint32_t readSize)
{
    TDPPACK *tpack;
     tpack=(TDPPACK *)rawBin;
    if(readSize<tpack->headSize)
    {   printf("tdp size erro!\r\n");
        return 0;
    }
  
    if(readSize<tpack->size+tpack->headSize)
    {
           return 0;
    }
    if(tpack->attr!=ATTR_VAPOR)
    { printf("require %d,but this one is %d\r\n",ATTR_VAPOR,tpack->attr);
        return 0;
    }
    return 1;
}/*name:getVaporType
description:to check if a rawBin reports a vapor,a and return its type
input:uint8_t *rawBin,uint32_t readSize
output:uint8_t 0 for illegaluint32_t *codeSize*/
uint8_t getVaporType(uint8_t *rawBin,uint32_t readSize)
{   if(isVapor(rawBin,readSize)!=1)
    {return 0;}
    TDPPACK *tpack=(TDPPACK *)rawBin;
    if(readSize<tpack->size+tpack->headSize)
    { printf("tdp size erro!\r\n");
           return 0;
    }
    if(tpack->attr!=ATTR_VAPOR)
    {   printf("require %d,but this one is %d\r\n",ATTR_VAPOR,tpack->attr);
        return 0;
    }
    return tpack->type;

}
/*name:vapor_transVcpuvars
description:transfer a bytestream of vcpuvars through vapor
input:NETSOCKET *netsock,VCPUVARS *cpuv
output:int8_t 1 for success
date:20200127
*/
int8_t vapor_transVcpuvars(NETSOCKET *netsock,VCPUVARS *cpuv)
{
    uint8_t *tdata;
    uint32_t dataSize;
    dataSize=vcpuvars_binSize(cpuv);
    tdata=vcpuvars_toNewBin(cpuv);
    if(tdata==0)
    {return -1;}
    if(vapor_transGeneric(netsock,tdata,dataSize,VAPOR_TYPE_CPUVARS)!=1)
    {return -1;}
    free(tdata);
    return 1;
}
/*name:vapor_getVcpuvars
description:get vcpuvars from a bytestream through vapor
input:uint8_t *rawBin,uint32_t readSize
output:VCPUVARS*
date:20200127
*/
VCPUVARS *vapor_getVcpuvars(uint8_t *rawBin,uint32_t readSize)
{
    VCPUVARS *tv;
    uint8_t *tdata;
    uint32_t dataBinSize;
    tdata=vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_CPUVARS,&dataBinSize);
    if(tdata==0)
    {return 0;}
    tv=(VCPUVARS *)malloc(sizeof(VCPUVARS));
    if(tv==0)
    {return 0;}
   if(vcpuvars_newFromBin(tv,tdata,dataBinSize)!=1)
    {free(tdata);return -1;}
    free(tdata);
    return tv;
}

/*name:vapor_transRunMem
description:transfer a bytestream of runTime Memory through vapor
input:NETSOCKET *netsock,uint8_t *data,uint32_t size
output:int8_t 1 for success
date:20200129
*/
int8_t vapor_transRunMem(NETSOCKET *netsock,uint8_t *data,uint32_t size)
{
 return vapor_transGeneric(netsock,data,size,VAPOR_TYPE_RUNMEM);
}

/*name:vapor_getRunMem
description:get a vapor run memory from rawBin,
input:uint8_t *rawBin,uint32_t readSize
output:uint8_t * for code, uint32_t for memSize
date:20200129
*/
uint8_t *vapor_getRunMem(uint8_t *rawBin,uint32_t readSize,uint32_t *memSize)
{
    return vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_RUNMEM,memSize);
}

/*name:vapor_reportUnitnode
description:transfer a bytestream of UNITNODE through vapor
input:NETSOCKET *netsock,UNITNODE *unode
output:int8_t 1 for success
date:20200204
*/
int8_t vapor_reportUnitnode(NETSOCKET *netsock,UNITNODE *unode)
{
    uint8_t *tdata;
    uint32_t dataSize;
    dataSize=unitnode_binSize(unode);
    tdata=unitnode_toNewBin(unode);
    if(tdata==0)
    {return -1;}
    if(vapor_transGeneric(netsock,tdata,dataSize,VAPOR_TYPE_UNITNODE)!=1)
    {return -1;}
    free(tdata);
    return 1;
}

/*name:vapor_getUnitnode
description:get unitnode from a bytestream through vapor
input:uint8_t *rawBin,uint32_t readSize
output:UNITNODE* 
date:20200204
*/
UNITNODE* vapor_getUnitnode(uint8_t *rawBin,uint32_t readSize)
{
    UNITNODE *tu;
    uint8_t *tdata;
    uint32_t dataBinSize;
    tdata=vapor_getGeneric(rawBin,readSize,VAPOR_TYPE_UNITNODE,&dataBinSize);
    if(tdata==0)
    {return 0;}
    tu=(VCPUVARS *)malloc(sizeof(UNITNODE));
    if(tu==0)
    {return 0;}
   if(unitnode_newFromBin(tu,tdata,dataBinSize)!=1)
    {free(tdata);return -1;}
    free(tdata);
    return tu;
}

/*name:vapor_sendWorkLoad
description:send a cpu's work load through vapor, in regard as certain netsocket
input:NETSOCKET *netsock
output:int8_t 1 for success
date:20200205
*/
int8_t vapor_sendWorkLoad(NETSOCKET *netsock,uint16_t workload)
{
     //first, generate the tdp
    TDPPACK s_tdp;
    TDPPACK *m_tdp=&s_tdp;
    m_tdp->headSize=sizeof(TDPPACK)-sizeof(uint8_t *);
    uint8_t tBin[sizeof(TDPPACK)];
    TDPPACK_addInfos(m_tdp,VAPOR_TYPE_WORKLOAD,ATTR_VAPOR,workload);
    m_tdp->data=0;
    m_tdp->size=0;
    //then, pack it
    TDPPACK_toExiBin(m_tdp,tBin);
    //next, send 
    netsocket_send(netsock,tBin,sizeof(TDPPACK));
    return 1;
}
/*name:vapor_getWorklLoad
description:to get a cpuwork load from vapor
input:uint8_t *rawBin,uint32_t readSize
output:uint16_t 
date:20200205*/
uint16_t vapor_getWorkLoad(uint8_t *rawBin,uint32_t readSize)
{
    uint16_t vwd;
    if(getVaporType(rawBin,readSize)!=VAPOR_TYPE_WORKLOAD)
        {return 0;}
    TDPPACK *tp=TDPPACK_fromExiBin(rawBin);
    vwd=tp->reserved&0xffff;
    TDPPACK_freePackNd(tp);
    return vwd;
}