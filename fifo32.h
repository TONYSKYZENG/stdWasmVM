#ifndef _FIFO32_H_
#define _FIFO32_H_
#include <stdint.h>
#include "list.h"
typedef struct FIFO32 {
	uint32_t * *buf;
	int32_t p, q, size, free, flags;
	ELIST datalist;
	// TASK *task;
}FIFO32;
void fifo32_init(FIFO32 *fifo, int32_t size, uint32_t *buf);
int32_t fifo32_put(FIFO32 *fifo, uint32_t data);
uint32_t fifo32_get(FIFO32 *fifo);
int32_t fifo32_status(FIFO32 *fifo);



#endif
