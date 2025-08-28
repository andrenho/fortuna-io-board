#ifndef PICO_QUEUE_H_
#define PICO_QUEUE_H_

#include "../pico.h"

typedef struct queue {

} queue_t;

void queue_init (queue_t *q, unsigned int element_size, unsigned int element_count);
bool queue_try_add (queue_t *q, const void *data);
bool queue_try_remove (queue_t *q, void *data);

#endif
