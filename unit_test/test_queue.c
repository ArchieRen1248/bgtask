#include <stdio.h>
#include <stdlib.h>
#include "../include/queue.h"

typedef struct _ActualQueueNode {
  QueueNode link;
  int value;
} ActualQueueNode;

void printSingleNode(const QueueNode *node) {
  ActualQueueNode *actualNode = (ActualQueueNode *)node;
  printf("%d\n", actualNode->value);
}

void createActualQueue(Queue *q, const int len) {
  for (int i = 0; i < len; i++) {
    ActualQueueNode *node = (ActualQueueNode *)malloc(sizeof(ActualQueueNode));
    node->value = i + 1;
    queuePushTail(q, (QueueNode *)node);
  }
}

void printQueue(const Queue queue) {
  slistForEach(queue.head, printSingleNode);
  printf("Node count of queue : %d\n", queue.cnt);
}

int main() {
  Queue queue = {.head = NULL, .tail = NULL, .cnt = 0};
  createActualQueue(&queue, 10);
  printQueue(queue);

  return 0;
}

