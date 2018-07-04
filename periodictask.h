#ifdef __cplusplus
extern "C" {
#endif

#ifndef PERIODICTASK_H_
#define PERIODICTASK_H_

#include "Arduino.h"
#include <Arduino_FreeRTOS.h>

struct _Parameters;  
typedef struct _Parameters Parameters;

struct _SchedulerParameters;
typedef struct _SchedulerParameters SchedulerParameters;

void periodicTask( void *param 
                  );

void taskCreater( void (*f)(void*),
                  const char * const pcName,
                  unsigned short usStackDepth,
                  void *pvParameters,
                  UBaseType_t uxPriority,
                  TaskHandle_t *pxCreatedTask,
                  TickType_t  frequency
                  );

TaskHandle_t *returnExecutingTask();

boolean isTaskReady(TaskHandle_t xHandle);

void populateTasksHandles(TaskHandle_t *pxHandle);

boolean isTaskReady(TaskHandle_t xHandle);

void schedulerTask( void *param );

void schedulerCreater( TaskHandle_t (*f)(void*),
                       void *param
                       );

#endif

#ifdef __cplusplus
}
#endif

