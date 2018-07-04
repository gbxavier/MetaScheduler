#include "periodictask.h"
#include <Arduino_FreeRTOS.h>
#include "Arduino.h"

#define MAX 10

TaskHandle_t schedulerHandleToTask;
UBaseType_t qtTasks = 0;
//TaskStatus_t *pxTaskStatusArray;
TaskHandle_t *tasksHandles[MAX];

struct _Parameters {
    void (*f)();                 // pointer to the function
    TickType_t xFrequency;
    void *funcParameters;
    TaskHandle_t* schedulerHandle;
};

struct _SchedulerParameters {
    TaskHandle_t (*f)();                 // pointer to the function
    void *funcParameters;
};

void periodicTask( void *param ) {

  Parameters*  pxParameters;
  pxParameters = (Parameters*) param;
  
  TaskHandle_t* schedulerHandle = pxParameters->schedulerHandle;
  
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pxParameters->xFrequency / portTICK_PERIOD_MS;

  // Initialise the xLastWakeTime variable with the current time.
  xLastWakeTime = xTaskGetTickCount();

  
  for(;;) {
    
    if ( uxTaskPriorityGet( NULL ) == 4 )
      vTaskPrioritySet(*schedulerHandle, 5);  //Aumenta a prioridade do escalonador pois nenhuma tarefa pode executar na prioridade 4

    // Here the function is instantiated.
    pxParameters->f(pxParameters->funcParameters);
    
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    
    vTaskPrioritySet(*schedulerHandle, 5);    //Aumenta a prioridade do escalonador com o intuito de chama-lo

  }
  vTaskDelete(NULL);
}

void taskCreater( void (*function)(void*),
                  const char * const pcName,
                  unsigned short usStackDepth,
                  void *pvParameters,
                  UBaseType_t uxPriority,
                  TaskHandle_t *pxCreatedTask,
                  TickType_t frequency
                  )
{
    /* Create the structure used to pass parameters to the first two tasks. */
  Parameters* pxParameters;
  pxParameters = (Parameters *) pvPortMalloc( sizeof( Parameters ) );
  
  if(pxCreatedTask == NULL){

    pxCreatedTask = (TaskHandle_t *) pvPortMalloc( sizeof( TaskHandle_t *) );
    
    
  }
  
  populateTasksHandles(pxCreatedTask);
  
  if( pxParameters != NULL )
  {
    /* Create the variable which is to be shared by the task. */
    pxParameters->f = (*function);
    pxParameters->xFrequency = frequency;
    pxParameters->funcParameters = pvParameters;
    pxParameters->schedulerHandle = &schedulerHandleToTask;

    /* Spawn the task. */
    xTaskCreate( periodicTask, 
                &*pcName, 
                usStackDepth, 
                pxParameters, 
                1,       //uxPriority, 
                &*pxCreatedTask
                ); 
  }
}

TaskHandle_t *returnExecutingTask(){

  TaskHandle_t *executingTask = NULL;
  
  for( int x = 0; x < qtTasks; x++ ){
    if( uxTaskPriorityGet( *tasksHandles[x] ) == 3 )
      executingTask = tasksHandles[x];
  }
  return executingTask;

}

void populateTasksHandles(TaskHandle_t *pxHandle){

    tasksHandles[qtTasks] = pxHandle;
    qtTasks++;
    
}

boolean isTaskReady(TaskHandle_t xHandle){
  return eTaskGetState( xHandle ) == 1;
}

void schedulerTask( void *param ) {
  
  SchedulerParameters*  sParameters;
  sParameters = (SchedulerParameters*) param;
  TaskHandle_t taskToExecuteNow;
  TaskHandle_t *taskExecuting = NULL;
  
  for(;;){

    if ( uxTaskPriorityGet( NULL ) == 2){
    
      taskExecuting = returnExecutingTask();
      if ( taskExecuting != NULL )
        vTaskPrioritySet( *taskExecuting , 4 );
    
    }
    
    taskToExecuteNow = sParameters->f( sParameters->funcParameters ); //Esta funcao deve retornar um HandleTask (Ainda nao implementado)
    
    if ( taskToExecuteNow == NULL ) {
      taskExecuting = returnExecutingTask();
      if ( taskExecuting != NULL )
        vTaskPrioritySet( *taskExecuting , 4 );
        
    }else{
      
      taskExecuting = returnExecutingTask();
        
      if ( uxTaskPriorityGet( NULL ) == 2){
        //Tarefa na prioridade 3 terminou
        if ( taskExecuting != NULL )
          vTaskPrioritySet( *taskExecuting , 4 );
        
        
        vTaskPrioritySet( taskToExecuteNow , 3 );
        
      }else{
      
        //Tarefa na prioridade 4 esta pronta e tarefa na prioridade 3 pode nao ter terminado sua execucao.
        if ( taskExecuting != NULL )
          vTaskPrioritySet( *taskExecuting , 1 );
          
        vTaskPrioritySet( taskToExecuteNow , 3 );
        vTaskPrioritySet( NULL , 2 );
        
      }
    
    }

  }
  
  vTaskDelete( NULL );
  
}

void schedulerCreater( TaskHandle_t (*f)(void*),
                       void *param
                       )
{
  
  SchedulerParameters* sParameters;
  sParameters = (SchedulerParameters *) pvPortMalloc( sizeof( SchedulerParameters ) );
  //populateTasksHandles();  

  if( sParameters != NULL )
  {
    sParameters->f = (*f);
    sParameters->funcParameters = param;
    
    // Create a task for the Scheduler
    xTaskCreate(  schedulerTask,
                  "Scheduler",
                  configMINIMAL_STACK_SIZE,
                  sParameters,
                  2,
                  &schedulerHandleToTask
                  ); 
  
  
    vTaskStartScheduler();
  }
}


