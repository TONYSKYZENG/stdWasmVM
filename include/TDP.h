#ifndef _TDP_H_
#define _TDP_H_
/*
#undef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif*/
#ifdef __cplusplus
extern "C"
{
#endif
#include <stdlib.h>
#define STHNEW(n) calloc(1,n)
#define STHFREE(n) free(n)
#include <stdint.h>
#undef NULL
#define N_ID 8
//structres 
typedef struct TDPPACK
{
uint8_t sender_id[N_ID];
uint8_t receiver_id[N_ID];
uint32_t size;
uint8_t type;
uint8_t attr;/*��������*/
uint64_t reserved;
uint8_t *data;
}TDPPACK;
//defines 0 TO 20 are basic TDP attr, 21 to 255 are user-defined
#define TYPE_NORMAL 0
#define TYPE_HANDSHAKE 1
#define TYPE_HEART 1
//functions
TDPPACK *TDPPACK_genNullPack(void);
//�������һ����
uint8_t TDPPACK_freePack(TDPPACK * pp);
//���һ������������ԭ������
uint8_t* TDPPACK_freePackNd(TDPPACK * pp);

//�����򵥵İ���������ֱ����ָ��ָ��ȥ
TDPPACK *TDPPACK_genSimplePack(uint8_t *sid,uint8_t *rid,uint32_t dsize,uint8_t *data);
//�������ݸ��ư�������������ԭ���ݵ��ſ���
TDPPACK *TDPPACK_genCopyDataPack(uint8_t *sid,uint8_t *rid,uint32_t dsize,uint8_t *data);
//ճ��һ��������
TDPPACK *TDPPACK_genPaste(TDPPACK * pp);
TDPPACK *TDPPACK_genPasteNd(TDPPACK * pp);
//�������и�������
void TDPPACK_addInfos(TDPPACK *pp,uint8_t type,uint8_t attr,uint64_t reserved);
//������½��Ķ�������
uint8_t *TDPPACK_toNewBin(TDPPACK *pp);
//�����һ���Ѿ����ڵĶ�������
uint8_t TDPPACK_toExiBin(TDPPACK *pp,uint8_t *outtr);
//��һ��ԭʼ��������������
TDPPACK *TDPPACK_fromExiBin(uint8_t *intr);
//ͬ�ϣ���ɾ��ԭʼ��
TDPPACK *TDPPACK_fromExiBinDel(uint8_t *intr);
//CHECKS
uint8_t TDPPACK_checkBinSize(uint8_t *rawbin,uint32_t psize);
uint8_t TDPPACK_checkBinType(uint8_t *rawbin,uint8_t type);
uint8_t TDPPACK_checkBinRID(uint8_t *rawbin,uint8_t *rid);
//special packs
TDPPACK *TDPPACK_genHandShakePack(uint8_t *sid,uint8_t *rid);
#ifdef __cplusplus
}
#endif


#endif

