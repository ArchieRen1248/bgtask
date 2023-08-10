#include <stdlib.h>
#include <stdio.h>
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

int main() {
  const int pTaskNum = 10;
  for (int i = 0; i < pTaskNum; i++) {
    PrintBGTask *pTask = createNewEmptyPrintBGTask(i + 1);
    bgtaskRegisterTask((BGTask *)&(pTask->task));
    bgtaskTriggerOneTask((BGTask *)&(pTask->task));
  }
  bgtaskStartSchedule();

  return 0;
}
