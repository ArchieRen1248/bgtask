#include <stdlib.h>
#include <stdio.h>
#include "../include/dlist.h"

typedef struct _ActualDList {
  DListNode link;
  int value1;
  int value2;
} ActualDList;

void printActualDListNode(const DListNode *node) {
  ActualDList *actualNode = (ActualDList *)node;
  printf("%d, %d\n", actualNode->value1, actualNode->value2);
}

void createActualDList(DListNode *dummyHead, const int len) {
  DListNode *node = dummyHead;
  for (int i = 0; i < len; i++) {
    DListNode *newNode = (DListNode *)malloc(sizeof(ActualDList));
    newNode->next = NULL;
    newNode->pre = NULL;
    ActualDList *actualNode = (ActualDList *)newNode;
    actualNode->value1 = i + 1;
    actualNode->value2 = i + 1;
    dlistInsertAfter(node, newNode);
    node = newNode;
  }
}

void freeActualDListNode(const DListNode *node) {
  free((ActualDList *)node);
}

int main() {
  DListNode *headNode = (DListNode *)malloc(sizeof(ActualDList));
  headNode->next = NULL;
  headNode->pre = NULL;

  DListNode *tailNode = (DListNode *)malloc(sizeof(ActualDList));
  tailNode->next = NULL;
  tailNode->pre = NULL;

  dlistInsertAfter(headNode, tailNode);

  createActualDList(headNode, 10);
  printf("Print forward: \n");
  dlistForEachForward(headNode, printActualDListNode);
  printf("Print backward: \n");
  dlistForEachBackward(tailNode, printActualDListNode);

  dlistForEachForward(headNode, freeActualDListNode);

  return 0;
}
