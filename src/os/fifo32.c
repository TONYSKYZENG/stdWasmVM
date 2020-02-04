#include "fifo32.h"
#define FLAGS_OVERRUN 0xfff
void fifo32_init(FIFO32 *fifo, int32_t size, uint32_t *buf)

{

	fifo->size = size;

	fifo->buf = buf;

	fifo->free = size; 

	fifo->flags = 0;

	fifo->p = 0; 

	fifo->q = 0; 

	list_init(&fifo->datalist);

	return;

}
int32_t fifo32_put(FIFO32 *fifo, uint32_t data)


{

		if (fifo->free == 0) {

		/* нч©у╪Д*/

		fifo->flags |= FLAGS_OVERRUN;

		return -1;

	}

	fifo->buf[fifo->p] = data;

	fifo->p++;

	if (fifo->p == fifo->size) {

		fifo->p = 0;

	}

	fifo->free--;

	return 0;

}

uint32_t fifo32_get(FIFO32 *fifo)

{

	uint32_t data;

	if (fifo->free == fifo->size) {

	

		return -1;

	}

	data = fifo->buf[fifo->q];

	fifo->q++;

	if (fifo->q == fifo->size) {

		fifo->q = 0;

	}

	fifo->free++;

	return data;

}

int32_t fifo32_status(FIFO32 *fifo)

{

		return fifo->size - fifo->free;

}

