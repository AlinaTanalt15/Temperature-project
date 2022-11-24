#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>

#include <semphr.h>


int Data;
SoftwareSerial BTserial(10, 9); // RX 10 - TX 9;




// Declarare Task-uri
void TaskTrimitereDate( void *pvParameters );
void TaskCitireTemperatura( void *pvParameters );


// Globals

char transmiteret[20]; 
int temperatureData; 



SemaphoreHandle_t xSerialSemaphore;

void setup() {
  // initialize serial communication at 9600 bits per second:

  Serial.begin(9600);
  BTserial.begin(9600);

  while (!Serial) {
    ;
  } 
   if (xSerialSemaphore == NULL)                // se verifica daca semaforul nu a fost deja creeat
   {
    xSerialSemaphore = xSemaphoreCreateMutex(); // se creeaza un semafor mutex, pentru port serial
    if ((xSerialSemaphore) != NULL)
    xSemaphoreGive((xSerialSemaphore));         // portul serial este disponibil pentru a fi folosit
    }

  xTaskCreate(
    TaskTrimitereDate
    ,  "task1"
    ,  128
    ,  NULL
    ,  1
    ,  NULL );
  xTaskCreate(
    TaskCitireTemperatura
    ,  "task2"
    ,  128
    ,  NULL
    ,  1
    ,  NULL );
   

  vTaskStartScheduler();
  
}

void loop()
{
}

void TaskTrimitereDate(void *pvParameters)  {
  pinMode(7, OUTPUT);
  while (1)
  {   
   if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
     {
     sprintf(transmiteret, "Temperatura:%d", temperatureData);// buffer pentru stocarea temperaturii
     
	xSemaphoreGive(xSerialSemaphore);
	}
     vTaskDelay(10);
    
  
     Serial.write(transmiteret);//afisarea temperaturii pe seriala
     vTaskDelay(100);
     Serial.println();
    
     Serial.println("Waiting for command...");
    
     if (BTserial.available()) { //wait for data received
      Data = BTserial.read();
      if (Data == '1') {
         BTserial.print("Temperatura este: ");
      if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
       {  
         BTserial.print(temperatureData);
	
         xSemaphoreGive(xSerialSemaphore);}  
         vTaskDelay(10);
         BTserial.println();
 
      }
      else if (Data == '2') {
        
        digitalWrite(7, HIGH);
        BTserial.print("LED Verde Aprins ");
        Serial.println("LED Verde Aprins ");
        
      }
      else if (Data == '3') {
    
        digitalWrite(6, HIGH);
        BTserial.print("LED Rosu Aprins ");
        Serial.println("LED Rosu Aprins ");
  
      }
     else if (Data == '4') {
      
      digitalWrite(7, LOW);
      digitalWrite(6, LOW);
      BTserial.print("LED-uri stinse ");
      Serial.println("LED-uri stinse ");
    
   
     }
     
    }
      vTaskDelay(500 / portTICK_PERIOD_MS );
  }
}

void TaskCitireTemperatura(void *pvParameters)  {
  while (1)
  {  if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
       { 
        
         int chk= analogRead(A0);
         float tempC = chk / 9.31;
         temperatureData=tempC;
     xSemaphoreGive(xSerialSemaphore);}
     vTaskDelay(10);
  }
  vTaskDelay(1500 / portTICK_PERIOD_MS );

}
