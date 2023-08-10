#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "dlist.h"
#include "queue.h"

#include <stdio.h>

#define BGTASK_MAX_RUN_CYCLE_TIMES 5

typedef enum _BGTaskStatus {
  BGTASK_STATUS_RUN = 0,
  BGTASK_STATUS_WAIT,
} BGTaskStatus;

typedef enum _BGTaskRuntimeStatus {
  BGTASK_RUNTIME_STATUS_CONTINUE = 0,
  BGTASK_RUNTIME_STATUS_FINISH,
} BGTaskRuntimeStatus;

typedef struct _BGTaskOps BGTaskOps;
typedef uint32_t BGTaskID;

typedef struct _BGTask {
  DListNode *link;
  BGTaskID id;
  BGTaskOps *ops;
  BGTaskStatus status;
} BGTask;

typedef struct _BGTaskOps {
  BGTaskRuntimeStatus (*fSchedule)(BGTask *);
  BGTaskRuntimeStatus (*fFinish)(BGTask *);
} BGTaskOps;

typedef struct _BGTaskMsgNode {
  QueueNode link;
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
BGTask *bgtaskCreateTask(BGTaskOps *ops, const BGTaskStatus status) {
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
static void removeFromRunList(BGTask *task) {
  assert(task != NULL);
  dlistRemoveNode((DListNode *)task);
  gBGTaskMgr.runTaskCnt--;
}

static void removeFromWaitList(BGTask *task) {
  assert(task != NULL);
  dlistRemoveNode((DListNode *)task);
  gBGTaskMgr.waitTaskCnt--;
}

void bgtaskUnregisterTask(BGTask *task) {
  if (task->status == BGTASK_STATUS_RUN) {
    removeFromRunList((BGTask *)task);
  }
  else if (task->status == BGTASK_STATUS_WAIT) {
    removeFromWaitList((BGTask *)task);
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
  dlistRemoveNode((DListNode *)task);
  dlistInsertBefore(gBGTaskMgr.runList, (DListNode *)task);
}

static BGTaskMsgNode *createNewBGTaskMsgNode(BGTask *task) {
  BGTaskMsgNode *msg = (BGTaskMsgNode *)malloc(sizeof(BGTaskMsgNode));
  msg->task = task;
}

void bgtaskTriggerOneTask(BGTask *task) {
  if (task->status == BGTASK_STATUS_WAIT) {
    moveTaskFromWaitToRun(task);
  }
  BGTaskMsgNode *msg = createNewBGTaskMsgNode(task);
  queuePushTail(&(gBGTaskMgr.msgQue), (QueueNode *)msg);
}

// Start schedule
void bgtaskStartSchedule() {
  while (gBGTaskMgr.msgQue.cnt > 0) {
    BGTaskMsgNode *msg = (BGTaskMsgNode *)queuePopHead(&(gBGTaskMgr.msgQue));
    BGTask *task = msg->task;
    assert(task->status == BGTASK_STATUS_RUN);
    uint32_t cycleTimes = 0;
    BGTaskRuntimeStatus ret = BGTASK_RUNTIME_STATUS_CONTINUE;
    do {
      ret = task->ops->fSchedule(task);
      cycleTimes++;
    } while (ret == BGTASK_RUNTIME_STATUS_CONTINUE && cycleTimes <= BGTASK_MAX_RUN_CYCLE_TIMES);
    if (ret == BGTASK_RUNTIME_STATUS_FINISH) {
      task->ops->fFinish(task);
    }
  }
}
