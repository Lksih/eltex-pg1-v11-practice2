#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_PRIORITY 255
#define PRIORITY_QUEUE_CAPACITY_INCREASE_STEP 10

typedef struct {
    uint8_t priority;
    void* data;
} pq_element;

typedef struct priority_queue {
    pq_element* elements;
    size_t count;
    size_t capacity;
} priority_queue;

priority_queue* pq_init(size_t initial_capacity);
int pq_enqueue(priority_queue* pq, void* data, uint8_t priority);
void* pq_dequeue(priority_queue* pq);
void* pq_dequeue_with_priority(priority_queue* pq, uint8_t priority);
void* pq_dequeue_with_min_priority(priority_queue* pq, uint8_t min_priority);
void pq_free(priority_queue* pq);

#endif