#include <lib/ringbuf.h>
#include <lib/string.h>
#include <stddef.h>

int ringbuf_push(ringbuf *rb, const void *item) {
	if (rb->count == rb->capacity)
		return -1;
	memcpy(rb->head, item, rb->size);
	rb->head = (char*)rb->head + rb->size;
	if (rb->head == rb->buf_end)
		rb->head = rb->buf;
	rb->count++;
	return 0;
}

int ringbuf_pop(ringbuf *rb, void *item) {
	if (rb->count == 0)
		return -1;
	memcpy(item, rb->tail, rb->size);
	rb->tail = (char*)rb->tail + rb->size;
	if (rb->tail == rb->buf_end)
		rb->tail = rb->buf;
	rb->count--;
	return 0;
}
