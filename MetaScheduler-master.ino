#include <Arduino_FreeRTOS.h>
#include "periodictask.h"
#include <math.h>

#define NUM_TASK 2

struct TaskProperties {
    TaskHandle_t *taskHandle;
    int period;
};
typedef struct TaskProperties TaskProperties;


void oneTask( void *param ) {

  UBaseType_t prioridade = uxTaskPriorityGet( NULL );

  Serial.println("T1. ");
  Serial.print("Priority ");
  Serial.println(prioridade);
  Serial.println("T1.. ");

}

void twoTask( void *param ) {

  UBaseType_t prioridade = uxTaskPriorityGet( NULL );

  Serial.println("T2.");
  Serial.print("Priority ");
  Serial.println(prioridade);
  Serial.println("T2..");

}

TaskHandle_t rateMonotonic(void *param) {

  Serial.println("Escalonador.");
  TaskProperties *proper = (TaskProperties *) param;
  double t1 = millis();
  while (millis() < t1 + 300) {
  
  }
  
  TaskHandle_t *candidate = NULL;
  int candidate_period = INFINITY;
  
  for (int x = 0 ; x < NUM_TASK ; x++) {

    if (isTaskReady(*proper[x].taskHandle)){
      
      if(proper[x].period < candidate_period){
        
        candidate_period = proper[x].period;
        candidate = proper[x].taskHandle;
      
      }
      
    }
    
  }
  Serial.println("Escalonador saiu...");
  return *candidate;

}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  
  TaskHandle_t handleTask1;
  TaskHandle_t handleTask2;
  TaskProperties properties[NUM_TASK];
  TaskProperties *proper;
  proper = properties;
  
  properties[0].taskHandle=&handleTask1;
  properties[0].period=1;
  properties[1].taskHandle=&handleTask2;
  properties[1].period=2;
  

  taskCreater(oneTask, "T1", configMINIMAL_STACK_SIZE, NULL, 2, &handleTask1 , 4000);
  taskCreater(twoTask, "T2", configMINIMAL_STACK_SIZE, NULL, 2, &handleTask2 , 2000);
  schedulerCreater(rateMonotonic, (void *) &properties);

}

void loop() {
  // put your main code here, to run repeatedly:

}
