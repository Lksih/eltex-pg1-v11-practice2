#include "priority_queue.h"
#include <string.h>

priority_queue *pq_init(size_t initial_capacity)
{
    if (initial_capacity == 0)
    {
        initial_capacity = PRIORITY_QUEUE_CAPACITY_INCREASE_STEP;
    }

    priority_queue *pq = (priority_queue *)malloc(sizeof(priority_queue));
    if (!pq)
        return NULL;

    pq->elements = (pq_element *)malloc(sizeof(pq_element) * initial_capacity);
    if (!pq->elements)
    {
        free(pq);
        return NULL;
    }

    pq->count = 0;
    pq->capacity = initial_capacity;

    return pq;
}

int pq_resize(priority_queue *pq)
{
    size_t new_capacity = pq->capacity + PRIORITY_QUEUE_CAPACITY_INCREASE_STEP;
    pq_element *new_elements = (pq_element *)realloc(pq->elements, sizeof(pq_element) * new_capacity);
    if (!new_elements)
    {
        return -1;
    }

    pq->elements = new_elements;
    pq->capacity = new_capacity;
    return 0;
}

int pq_enqueue(priority_queue *pq, void *data, uint8_t priority)
{
    if (!pq)
    {
        return -1;
    }

    if (pq->count >= pq->capacity)
    {
        if (pq_resize(pq) != 0)
        {
            return -1;
        }
    }

    pq->elements[pq->count].data = data;
    pq->elements[pq->count].priority = priority;
    pq->count++;

    return 0;
}

int find_element_index_priority_equal(priority_queue *pq, uint8_t priority)
{
    if (!pq)
    {
        return -1;
    }

    for (size_t i = 0; i < pq->count; i++)
    {
        if (pq->elements[i].priority == priority)
        {
            return (int)i;
        }
    }

    return -1;
}

int find_element_index_priority_equal_or_more(priority_queue *pq, uint8_t priority)
{
    if (!pq)
    {
        return -1;
    }

    for (size_t i = 0; i < pq->count; i++)
    {
        if (pq->elements[i].priority >= priority)
        {
            return (int)i;
        }
    }

    return -1;
}

void remove_element_at_index(priority_queue *pq, size_t index)
{
    if (index >= pq->count)
    {
        return;
    }

    memmove(&pq->elements[index], &pq->elements[index + 1],
            sizeof(pq_element) * (pq->count - index - 1));
    pq->count--;
}

void *pq_dequeue(priority_queue *pq)
{
    if (!pq || pq->count == 0)
    {
        return NULL;
    }

    void *data = pq->elements[0].data;
    remove_element_at_index(pq, 0);

    return data;
}

void *pq_dequeue_with_priority(priority_queue *pq, uint8_t priority)
{
    int idx = find_element_index_priority_equal(pq, priority);
    if (idx == -1)
    {
        return NULL;
    }

    void *data = pq->elements[idx].data;
    remove_element_at_index(pq, idx);

    return data;
}

void *pq_dequeue_with_min_priority(priority_queue *pq, uint8_t min_priority)
{
    int idx = find_element_index_priority_equal_or_more(pq, min_priority);
    if (idx == -1)
    {
        return NULL;
    }

    void *data = pq->elements[idx].data;
    remove_element_at_index(pq, idx);

    return data;
}

void pq_free(priority_queue *pq)
{
    if (pq)
    {
        free(pq->elements);
        free(pq);
    }
}