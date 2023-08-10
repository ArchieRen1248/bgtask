#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "../include/bgtask.h"

typedef struct _PrintBGTask {
  BGTask task;
  int printValue;
} PrintBGTask;

BGTaskRuntimeStatus printBGTaskSchedule(BGTask *task) {
  PrintBGTask *pTask = (PrintBGTask *)task;
  printf("PrintBGTask print : %d\n", pTask->printValue);
  return BGTASK_RUNTIME_STATUS_FINISH;
}

BGTaskRuntimeStatus printBGTaskFinish(BGTask *task) {
  PrintBGTask *pTask = (PrintBGTask *)task;
  printf("PrintBGTask with value %d stop.\n", pTask->printValue);
  return BGTASK_RUNTIME_STATUS_FINISH;
}

BGTaskOps gPrintBGTaskOps = {.fSchedule = printBGTaskSchedule,
                             .fFinish = printBGTaskFinish,
                            };

PrintBGTask *createNewEmptyPrintBGTask(int printValue) {
  PrintBGTask *pTask = (PrintBGTask *)malloc(sizeof(PrintBGTask));
  pTask->task.link = NULL;
  pTask->task.id; // TODO
  pTask->task.ops = &gPrintBGTaskOps;
  pTask->task.status = BGTASK_STATUS_RUN;
  pTask->printValue = printValue;
}

void freePrintBGTask(void *pTask) {
  free((PrintBGTask *)pTask);
}

static volatile int bgtaskIdx = 0;

void *bgtaskCreatorThread() {
  bgtaskIdx = 0;
  while (1) {
    sleep(1);
    PrintBGTask *pTask = createNewEmptyPrintBGTask(bgtaskIdx++);
    bgtaskRegisterTask((BGTask *)&(pTask->task));
    bgtaskTriggerOneTask((BGTask *)&(pTask->task));
  }
}

void *bgtaskProcessThread() {
  int flag = 1;
  const int processGap = 3;
  while (1) {
    flag++;
    sleep(1);
    if (flag > processGap) {
      flag %= processGap;
      bgtaskStartSchedule();
    }
  }
}

typedef void *(*ThreadFunc_t)();

int main() {
  // Thread num definition
  const int threadNum = 3, creatorThreadNum = 2, processThreadNum = 1;
  assert(creatorThreadNum + processThreadNum == threadNum);

  pthread_t threadId[threadNum];
  int threadRet = 0;
  
  // Create and run create thread
  for (int i = 0; i < creatorThreadNum; i++) {
    threadRet = pthread_create(&threadId[i], NULL, bgtaskCreatorThread, NULL);
    if (threadRet == -1) {
      printf("Create thread [%d] error.\n", i);
      return 1;
    }
  }

  // Create and run process thread
  for (int i = 0; i < creatorThreadNum; i++) {
    threadRet = pthread_create(&threadId[i], NULL, bgtaskProcessThread, NULL);
    if (threadRet == -1) {
      printf("Create thread [%d] error.\n", i);
      return 1;
    }
  }

  // Thread join
  for (int i = 0; i < threadNum; i++) {
    pthread_join(threadId[i], NULL);
  }

  return 0;
}

