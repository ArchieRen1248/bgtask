#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <assert.h>
#include "slist.h"

typedef SListNode QueueNode;

typedef struct _Queue {
    QueueNode *head;
    QueueNode *tail;
    uint32_t cnt;
} Queue;

void *queuePeek(const Queue *q) {
    return (void *)(q->head);
}

void *queuePopHead(Queue *q) {
    if (q->head == NULL) {
        return NULL;
    }
    QueueNode *node = q->head;
    q->head = q->head->next;
    q->cnt--;
    assert(q >= 0);
    if (q->tail == node) {
      q->tail = q->head;
    }
    return node;
}

void *queuePushTail(Queue *q, QueueNode *node) {
    assert(node != NULL);
    if (q->head == NULL) {
        q->head = node;
        q->tail = node;
    } else {
        q->tail->next = node;
        q->tail = node;
    }
    q->cnt++;
}

#endif
