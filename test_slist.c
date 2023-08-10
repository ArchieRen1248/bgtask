#include <stdio.h>
#include <stdlib.h>
#include "../include/slist.h"

typedef struct _ActualSList {
  SListNode link;
  int value1;
  int value2;
} ActualSList;

void createActualList(SListNode *head, const int len) {
  SListNode *node = head;
  for (int i = 0; i < len; i++) {
    SListNode *newNode = (SListNode *)malloc(sizeof(ActualSList));
    ActualSList *actualNode = (ActualSList *)newNode;
    actualNode->value1 = i + 1;
    actualNode->value2 = i + 1;
    slistInsertAfter(node, newNode);
    node = newNode;
  }
}

void freeActualListNode(const SListNode *node) {
  free((ActualSList *)node);
}

void printActualListNode(const SListNode *node) {
  ActualSList *actualNode = (ActualSList *)node;
  printf("%d, %d\n", actualNode->value1, actualNode->value2);
}

int main() {
  SListNode *head = (SListNode *)malloc(sizeof(ActualSList));
  head->next = NULL;
  createActualList(head, 10);
  slistForEach(head, printActualListNode);

  // Free memory
  slistForEach(head, freeActualListNode);
  
  return 0;
}
