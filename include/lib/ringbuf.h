#pragma once
#include <stddef.h>

typedef struct
{
	void *buf;
	void *buf_end;
	size_t capacity;
	size_t count;
	size_t size;
	void *head;
	void *tail;
} ringbuf;

#define RINGBUF_STATIC(name, elem_count, elem_type) \
struct { \
ringbuf rb; \
elem_type buf[elem_count]; \
} name##_inst = { \
.rb = { \
.buf = (void*)name##_inst.buf, \
.buf_end = (char*)name##_inst.buf + sizeof(name##_inst.buf), \
.capacity = elem_count, \
.count = 0, \
.size = sizeof(elem_type), \
.head = (void*)name##_inst.buf, \
.tail = (void*)name##_inst.buf \
} \
}; \
ringbuf *name = &name##_inst.rb;

int ringbuf_push(ringbuf *rb, const void *item);
int ringbuf_pop(ringbuf *rb, void *item);