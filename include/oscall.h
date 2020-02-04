#ifndef _OSCALL_H_
#define _OSCALL_H_
#include <stdlib.h>
#include <string.h>
#include "wasmvm.h"
#define IDX_API_NEWTHREAD 2
#define IDX_API_DELTHREAD 3
#define IDX_ADI_GETRUNID 4
uint32_t os_apiP3R32(uint32_t idx,void *p1,void *p2,Module *m);

#endif
