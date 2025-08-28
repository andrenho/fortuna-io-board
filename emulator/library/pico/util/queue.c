#include "queue.h"

#include <stdlib.h>

/**
 * Initialize a queue with specified element size and capacity
 * @param q Pointer to queue structure
 * @param element_size Size of each element in bytes
 * @param element_count Maximum number of elements the queue can hold
 */
void queue_init(queue_t *q, unsigned int element_size, unsigned int element_count) {
    if (q == NULL || element_size == 0 || element_count == 0) {
        return;
    }

    q->buffer = malloc(element_size * element_count);
    if (q->buffer == NULL) {
        // Handle allocation failure
        q->head = 0;
        q->tail = 0;
        q->size = 0;
        q->capacity = 0;
        q->elem_size = 0;
        return;
    }

    q->head = 0;
    q->tail = 0;
    q->size = 0;
    q->capacity = element_count;
    q->elem_size = element_size;
}

/**
 * Try to add an element to the queue
 * @param q Pointer to queue structure
 * @param data Pointer to data to be added
 * @return true if element was added successfully, false if queue is full or invalid
 */
bool queue_try_add(queue_t *q, const void *data) {
    if (q == NULL || data == NULL || q->buffer == NULL) {
        return false;
    }

    // Check if queue is full
    if (q->size >= q->capacity) {
        return false;
    }

    // Copy data to the tail position
    char *buffer_bytes = (char*)q->buffer;
    memcpy(buffer_bytes + (q->tail * q->elem_size), data, q->elem_size);

    // Update tail position (circular)
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;

    return true;
}

/**
 * Try to remove an element from the queue
 * @param q Pointer to queue structure
 * @param data Pointer to buffer where removed data will be copied
 * @return true if element was removed successfully, false if queue is empty or invalid
 */
bool queue_try_remove(queue_t *q, void *data) {
    if (q == NULL || data == NULL || q->buffer == NULL) {
        return false;
    }

    // Check if queue is empty
    if (q->size == 0) {
        return false;
    }

    // Copy data from the head position
    char *buffer_bytes = (char*)q->buffer;
    memcpy(data, buffer_bytes + (q->head * q->elem_size), q->elem_size);

    // Update head position (circular)
    q->head = (q->head + 1) % q->capacity;
    q->size--;

    return true;
}

