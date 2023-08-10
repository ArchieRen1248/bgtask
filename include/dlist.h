#ifndef __DLIST_H__
#define __DLIST_H__

#include <stdlib.h>

typedef struct _DListNode {
  struct _DListNode *pre;
  struct _DListNode *next;
} DListNode;

typedef void (*fDListOp)(const DListNode *);

void dlistInsertAfter(DListNode *currNode, DListNode *node) {
  DListNode *nextNode = currNode->next;
  currNode->next = node;
  node->pre = currNode;
  node->next = nextNode;
  if (nextNode != NULL) {
      nextNode->pre = node;
  }
}

void dlistInsertBefore(DListNode *currNode, DListNode *node) {
  DListNode *preNode = currNode->pre;
  currNode->pre = node;
  node->next = currNode;
  node->pre = preNode;
  if (preNode != NULL) {
      preNode->next = node;
  }
}

void dlistForEachForward(DListNode *headNode, const fDListOp op) {
  DListNode *node = headNode;
  while (node != NULL) {
    DListNode *nextNode = node->next;
    op(node);
    node = nextNode;
  }
}

void dlistForEachBackward(DListNode *endNode, const fDListOp op) {
  DListNode *node = endNode;
  while (node != NULL) {
    DListNode *preNode = node->pre;
    op(node);
    node = preNode;
  }
}

void dlistRemoveNode(DListNode *node) {
  assert(node != NULL);
  DListNode *preNode = node->pre, *nextNode = node->next;
  preNode->next = nextNode;
  if (nextNode != NULL) {
    nextNode->pre = preNode;
  }
  node->next = NULL;
  node->pre = NULL;
}

#endif
