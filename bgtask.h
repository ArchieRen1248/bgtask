#include <stdlib.h>
#include <stdint.h>
#include "dlist.h"
#include "queue.h"

typedef enum _BGTaskStatus {
  BGTASK_STATUS_RUN = 0,
  BGTASK_STATUS_WAIT,
} BGTaskStatus;

typedef struct _BGTaskOps BGTaskOps;
typedef uint32_t BGTaskID;

typedef struct _BGTask {
  DListNode *link;
  BGTaskID id;
  BGTaskOps *ops;
  BGTaskStatus status;
} BGTask;

typedef struct _BGTaskOps {
  void (*fStart)(BGTask *);
  void (*fSchedule)(BGTask *);
  void (*fFinish)(BGTask *);
} BGTaskOps;

typedef struct _MsgQue {
  QueueNode link;
  BGTask *task;
} MsgQue;

typedef struct _BGTaskMsgNode {
  QueueNode *link;
  BGTask *task;
} BGTaskMsgNode;

typedef struct _BGTaskMgr {
  DListNode *runList;
  DListNode *waitList;
  uint32_t runTaskCnt;
  uint32_t waitTaskCnt;
  Queue msgQue;
} BGTaskMgr;

static BGTaskID gBGTaskBaseID = 0;
static BGTaskMgr gBGTaskMgr = {.runList = NULL,
                               .waitList = NULL,
                               .runTaskCnt = 0,
                               .waitTaskCnt = 0,
                               .msgQue = {.head = NULL, .tail = NULL, .cnt = 0},
                              };

// Create one bgtask
BGTask *bgtaskCreateTask(const BGTaskOps *ops, const BGTaskStatus status) {
  BGTask *task = (BGTask *)malloc(sizeof(BGTask));
  task->link = NULL;
  task->id = gBGTaskBaseID++;
  task->ops = ops;
  task->status = status;
  return task;
}

// Register
void bgtaskRegisterTask(BGTask *task) {
  if (gBGTaskMgr.runList == NULL) {
    assert(gBGTaskMgr.runTaskCnt == 0);
    gBGTaskMgr.runList = (DListNode *)task;
  } else {
    dlistInsertBefore(gBGTaskMgr.runList, (DListNode *)task);
  }
  gBGTaskMgr.runTaskCnt++;
}

// Unregister bgtask
static removeFromRunList(BGTask *task) {
  assert(task != NULL);
  dlistRemoveNode((DListNode *)task);
  gBGTaskMgr.runTaskCnt--;
}

static removeFromWaitList(BGTask *task) {
  assert(task != NULL);
  dlistRemoveNode((DListNode *)task);
  gBGTaskMgr.waitTaskCnt--;
}

void bgtaskUnregisterTask(BGTask *task) {
  if (task->status == BGTASK_STATUS_RUN) {
    removeFromRunList((DListNode *)task);
  }
  else if (task->status == BGTASK_STATUS_WAIT) {
    removeFromWaitList((DListNode *)task);
  }
  else {
    assert(0);
  }
  free(task);
}

// Trigger one bgtask
static void moveTaskFromWaitToRun(BGTask *task) {
  assert(task->status == BGTASK_STATUS_WAIT);
  task->status = BGTASK_STATUS_RUN;
  dlistRemoveNode(task);
  dlistInsertBefore(gBGTaskMgr.runList, (DListNode *)task);
}

void bgtaskTriggerOneTask(BGTask *task) {
  if (task->status == BGTASK_STATUS_WAIT) {
    moveTaskFromWaitToRun(task);
  }
  queuePushTail(gBGTaskMgr.queue, (QueueNode *)task);
}

// Start schedule
