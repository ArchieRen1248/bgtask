#ifndef __SLIST_H__
#define __SLIST_H__

#include <stdlib.h>

typedef struct _SListNode {
  struct _SListNode *next;
} SListNode;

typedef void (*fSListOp)(const SListNode *);

void slistInsertAfter(SListNode *currNode, SListNode *node) {
  SListNode *nextNode = currNode->next;
  currNode->next = node;
  node->next = nextNode;
}

void slistForEach(SListNode *headNode, const fSListOp op) {
  SListNode *node = headNode;
  while (node != NULL) {
    SListNode *newNode = node->next;
    op(node);
    node = newNode;
  }
}

#endif
