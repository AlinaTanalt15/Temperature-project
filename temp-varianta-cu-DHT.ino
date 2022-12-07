#include <Arduino_FreeRTOS.h>
#include <SoftwareSerial.h>
#include <dht.h>
#include <semphr.h>

#include <task.h>

dht DHT;
int Data;
SoftwareSerial BTserial(10, 9); // RX 10 - TX 9;
#define DHT11_PIN 4

// Declarare Task-uri
void TaskTrimitereDate( void *pvParameters );
void TaskCitireTemperatura( void *pvParameters );
void TaskCitireHumi( void *pvParameters );

// Globals
unsigned long timeBegin;
unsigned long timeEnd;
unsigned long duration;
double averageDuration;
unsigned long timeBegin1;
unsigned long timeEnd1;
unsigned long duration1;
double averageDuration1;
char transmiteret[20]; 
char transmitereh[20];
int temperatureData; 
int humiData;
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
    xTaskCreate(
    TaskCitireHumi
    ,  "task3"
    ,  128
    ,  NULL
    ,  2
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
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
     {
     sprintf(transmitereh, "Umi:%d", humiData);// buffer pentru stocarea umiditatii
     
	xSemaphoreGive(xSerialSemaphore);}
     vTaskDelay(10);
  
     Serial.write(transmiteret);//afisarea temperaturii pe seriala
    
     vTaskDelay(100);
     Serial.println();
     Serial.write(transmitereh);//afisarea umiditatii pe seriala
     vTaskDelay(100);
  
      Serial.println();
      Serial.print("Timpul executarii task-ului TaskCitireHumi este de: ");
      Serial.print(averageDuration);
       Serial.println("ms");
  
       Serial.print("Timpul executarii task-ului TaskCitireTemperatura este de: ");
      Serial.print(averageDuration1);
       Serial.println("ms");
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
     else if (Data == '5') {
      
      BTserial.print("Humi este: ");
      if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
       {  
         BTserial.print(humiData);
       
         xSemaphoreGive(xSerialSemaphore);} 
         vTaskDelay(10);
         BTserial.println();
    
   
     }
    }
      vTaskDelay(500 / portTICK_PERIOD_MS );
  }
}


void TaskCitireTemperatura(void *pvParameters)  {
  while (1)
  { 
      timeBegin1 = micros();
     int chk = DHT.read11(DHT11_PIN);//colectarea valorii de le senzor
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
       { 
     temperatureData = DHT.temperature; // citeste valoarea de temperatura de pe senzor
     
	
     xSemaphoreGive(xSerialSemaphore);}
     vTaskDelay(10);
     timeEnd1 = micros();
    duration1 = timeEnd - timeBegin;
    averageDuration1 = (double)duration1 / 1000 ;

  }
  vTaskDelay(1500 / portTICK_PERIOD_MS );

}
void TaskCitireHumi(void *pvParameters)  {
  while (1)
  { 
       timeBegin = micros();
     int chk1 = DHT.read11(DHT11_PIN);// colectarea valorii de la senzor
     if (xSemaphoreTake(xSerialSemaphore, (TickType_t) 5) == pdTRUE)
      { 
     humiData = DHT.humidity; // citeste valoarea de umiditate de pe senzor
      
     xSemaphoreGive(xSerialSemaphore);}
     vTaskDelay(10);
       timeEnd = micros();
       duration = timeEnd - timeBegin;
       averageDuration = (double)duration / 1000 ;
     
  }
  

  vTaskDelay(1500 / portTICK_PERIOD_MS ); 
}
