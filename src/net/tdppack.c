#include "TDP.h"
TDPPACK *TDPPACK_genNullPack(void)
{

TDPPACK *a;
a=(TDPPACK *)STHNEW(sizeof(TDPPACK));
a->headSize=sizeof(TDPPACK)-sizeof(uint8_t *);
return a;
}
uint8_t TDPPACK_freePack(TDPPACK * pp)
{
STHFREE(pp->data);
STHFREE(pp);
return 1;
}
uint8_t* TDPPACK_freePackNd(TDPPACK * pp)
{
uint8_t *d;
d=pp->data;
STHFREE(pp);
return d;
}

TDPPACK *TDPPACK_genSimplePack(uint8_t *sid,uint8_t *rid,uint32_t dsize,uint8_t *data)
{   int i,j;
	TDPPACK *a=TDPPACK_genNullPack();
    if(a==0)
		{
			return a;
		}
	for(i=0;i<N_ID;i++)
		{
			a->sender_id[i]=sid[i];
			a->receiver_id[i]=rid[i];
		}
	a->size=dsize;
	a->data=data;
	return a;
}
TDPPACK *TDPPACK_genCopyDataPack(uint8_t *sid,uint8_t *rid,uint32_t dsize,uint8_t *data)
{   uint32_t i;
	TDPPACK *a=TDPPACK_genNullPack();
    if(a==0)
		{
			return a;
		}
	for(i=0;i<N_ID;i++)
		{
			a->sender_id[i]=sid[i];
			a->receiver_id[i]=rid[i];
		}
	a->size=dsize;
	a->data=(uint8_t *)STHNEW(dsize);
	for(i=0;i<dsize;i++)
		{
			
			a->data[i]=data[i];
		}
	return a;
}

TDPPACK *TDPPACK_genPaste(TDPPACK * pp)
{
	TDPPACK *a;
	uint32_t i,j;
    if(pp==0)
		{
            return 0;
		}
	a=TDPPACK_genNullPack();
	a->size=pp->size;
	a->headSize=pp->headSize;//here!
		for(i=0;i<N_ID;i++)
		{
			a->sender_id[i]=pp->sender_id[i];
			a->receiver_id[i]=pp->sender_id[i];
		}
		a->reserved=pp->reserved;
		a->type=pp->type;
		a->attr=pp->attr;
		j=pp->size;
		a->data=(uint8_t *)STHNEW(j);
	for(i=0;i<j;i++)
		{
			
			a->data[i]=pp->data[j];
		}
      return a;

}
TDPPACK *TDPPACK_genPasteNd(TDPPACK * pp)
{
	TDPPACK *a;
	uint32_t i,j;
    if(pp==0)
		{
            return 0;
		}
	a=TDPPACK_genNullPack();
	a->size=pp->size;
	a->headSize=pp->headSize;//here!
		for(i=0;i<N_ID;i++)
		{
			a->sender_id[i]=pp->sender_id[i];
            a->receiver_id[i]=pp->receiver_id[i];
		}
		a->reserved=pp->reserved;
		a->type=pp->type;
		a->attr=pp->attr;
		j=pp->size;
		a->data=pp->data;
		return a;
}
void TDPPACK_addInfos(TDPPACK *pp,uint8_t type,uint8_t attr,uint64_t reserved)
{
pp->type=type;
pp->attr=attr;
pp->reserved=reserved;
}
uint8_t *TDPPACK_toNewBin(TDPPACK *pp)
{

	uint32_t allsize,i,ppsize,headsize;
	uint8_t *outtr,*ptr;
	ppsize=pp->size;
	headsize=pp->headSize;
	allsize=headsize+ppsize;
	ptr=(uint8_t *)pp;
    outtr=(uint8_t *)STHNEW(allsize);
	for(i=0;i<headsize;i++)
	{
		outtr[i]=ptr[i];
	}
	ptr=pp->data;
    for(i=0;i<ppsize;i++)
	{
		outtr[i+headsize]=ptr[i];
    }

	return outtr;
}

uint8_t TDPPACK_toExiBin(TDPPACK *pp,uint8_t *outtr)
{

uint32_t allsize,i,ppsize,headsize;
	uint8_t *ptr,*str;
	ppsize=pp->size;
	headsize=pp->headSize;
	allsize=headsize+ppsize;
	ptr=(uint8_t *)pp;
	for(i=0;i<headsize;i++)
	{
		outtr[i]=ptr[i];
	}
	ptr=pp->data;
    for(i=0;i<ppsize;i++)
	{
		outtr[i+headsize]=ptr[i];
    }


return 1;

}
TDPPACK *TDPPACK_fromExiBin(uint8_t *intr)
{  uint32_t i,j;
	TDPPACK *ptr=(TDPPACK *)intr;
	uint8_t *dtr;
	TDPPACK *a= TDPPACK_genPasteNd(ptr);
    if(a==0)
		{
			return a;
		}
	dtr=intr+ptr->headSize;
	j=a->size;
  	a->data=(uint8_t *)STHNEW(j);
	for(i=0;i<j;i++)
		{
			
			a->data[i]=dtr[i];
		}
	return a;

}
TDPPACK *TDPPACK_fromExiBinDel(uint8_t *intr)
{
	TDPPACK *ptr=TDPPACK_fromExiBin(intr);
    STHFREE(intr);
	return ptr;
}
uint8_t TDPPACK_checkBinSize(uint8_t *rawbin,uint32_t psize)
{   TDPPACK *tpack;
 tpack=(TDPPACK *)rawbin;
    if(psize<tpack->headSize)
    {
        return 0;
    }
  
    if(psize<tpack->size+tpack->headSize)
       {
           return 0;
       }
    return 1;


}
uint8_t TDPPACK_checkBinType(uint8_t *rawbin,uint8_t type)
{
    TDPPACK *tpack;

      tpack=(TDPPACK *)rawbin;
       if(tpack->type!=type)
          {
              return 0;
          }
       return 1;

}
uint8_t TDPPACK_checkBinRID(uint8_t *rawbin,uint8_t *rid)
{
    TDPPACK *tpack;
   tpack=(TDPPACK *)rawbin;
   uint32_t i;
   for(i=0;i<N_ID;i++)
   {
       if(tpack->receiver_id[i]!=rid[i])
       {return 0;}
   }
   return 1;
}

TDPPACK *TDPPACK_genHandShakePack(uint8_t *sid,uint8_t *rid)
{   TDPPACK *tp;
    uint8_t a='h';
    tp=TDPPACK_genCopyDataPack(sid,rid,1,&a);
    TDPPACK_addInfos(tp,TYPE_HANDSHAKE,0,0);
    return tp;
}
