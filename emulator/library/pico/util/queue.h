#ifndef PICO_QUEUE_H_
#define PICO_QUEUE_H_

#include "../../pico.h"

typedef struct queue {
    void *buffer;           // Buffer to store elements
    unsigned int head;      // Index of first element
    unsigned int tail;      // Index where next element will be added
    unsigned int size;      // Current number of elements
    unsigned int capacity;  // Maximum number of elements
    unsigned int elem_size; // Size of each element in bytes
} queue_t;

void queue_init (queue_t *q, unsigned int element_size, unsigned int element_count);
bool queue_try_add (queue_t *q, const void *data);
bool queue_try_remove (queue_t *q, void *data);

#endif
