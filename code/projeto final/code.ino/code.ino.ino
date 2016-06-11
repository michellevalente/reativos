/**
 * JY-MCU Bluetooth module communication example for Arduino.
 *
 * Connect RX_PIN to TX pin of the module,
 * Connect TX_PIN to RX pin of the module.
 *
 * Based on SoftwareSerial example by Tom Igoe and Mikal Hart.
 * Adapted for JY-MCU module by Tautvidas Sipavicius.
 *
 * This example code is in the public domain.
 */
 
#include <SoftwareSerial.h>
#include <string.h>
#include <math.h>
#include "Servo.h"

#define MAX_CODE 500
#define _offset  -50

#define SERVO_LEFT     13
#define SERVO_RIGHT    12
#define SENSOR_ONE_IN  10
#define SENSOR_ONE_OUT  9
#define LED_BLUE        8
#define LED_RED         7
#define SENSOR_TWO_IN   3
#define SENSOR_TWO_OUT  2
#define BUZZER          4

#define FREQ_RANGE_LOWER 38000
#define FREQ_RANGE_UPPER 42000

const int RX_PIN = 0;
const int TX_PIN = 1;
const int BLUETOOTH_BAUD_RATE = 9600;
 
SoftwareSerial bluetooth(RX_PIN, TX_PIN);
 
char code[MAX_CODE], reactCode[MAX_CODE];
int count = 0, reactCount = 0;
int code_pos = 0, reactCodePos = 0;
int loopPos  = -1, reactLoopPos = -1;
unsigned long last_show, initial;
boolean receivingCode = true;
boolean recvReact = false;
boolean isReacting = false;
Servo servoLeft;                             
Servo servoRight;

int sensorDistance(int sensorLedPin, int sensorReceivePin)
{  
  int distance = 0;
  for(long f = FREQ_RANGE_LOWER; f <= FREQ_RANGE_UPPER; f += 1000) {
    distance += sensorDetect(sensorLedPin, sensorReceivePin, f);
  }
  return distance;
}

int sensorDetect(int sensorLedPin, int sensorReceiverPin, long frequency)
{
  tone(sensorLedPin, frequency, 8);             
  delay(1);                                
  int sensorRead = digitalRead(sensorReceiverPin);       
  delay(1); 
  // Return 0 if detect something  
  return sensorRead;                                
}    

void setServo(int speedLeft, int speedRight, int Time)
{
  servoLeft.writeMicroseconds(1500 + speedLeft);  
  servoRight.writeMicroseconds(1500 - speedRight); 
  delay(Time);                                  
}

void setup() {
   Serial.begin(9600);
   bluetooth.begin(BLUETOOTH_BAUD_RATE);
   pinMode(SENSOR_ONE_IN, INPUT);  
   pinMode(SENSOR_ONE_OUT, OUTPUT);   
   pinMode(SENSOR_TWO_IN, INPUT);  
   pinMode(SENSOR_TWO_OUT, OUTPUT);    
   pinMode(LED_RED, OUTPUT);  
   pinMode(LED_BLUE, OUTPUT);  
   servoLeft.attach(SERVO_LEFT);                      
   servoRight.attach(SERVO_RIGHT); 
   last_show = millis();
   initial = millis();
   resetState();
}

int runCode()
{
  int i;
  char * curCode = code;
  int  *curCodePos = &code_pos, *curLoopPos = &loopPos, length = count;
  if (isReacting) {
    Serial.println("IS REACT");
    curCode = reactCode;
    curCodePos = &reactCodePos;
    curLoopPos = &reactLoopPos;
    length = reactCount;
  }
  
  if (*curCodePos < length) { 
      char command[5];
      for (i = 0; i < 4; i++)
        command[i] = curCode[(*curCodePos)++];
       command[i] = '\0';
       
      if (strcmp(command, "loop") == 0) {
         *curLoopPos = (*curCodePos);
      }
      else if (strcmp(command, "ledr") == 0) { // red led
           digitalWrite(LED_RED, curCode[(*curCodePos)++] == '1' ? HIGH : LOW); 
      }
      else if (strcmp(command, "ledb") == 0) { // blue led
           digitalWrite(LED_BLUE, curCode[(*curCodePos)++] == '1' ? HIGH : LOW); 
      }
      else if(strcmp(command,"buzz") == 0) { // buzzer
        int timer;
        sscanf(curCode + (*curCodePos), "%d", &timer);
        (*curCodePos) += floor(log10(abs(timer))) + 1;
        tone(BUZZER, 1500, timer);     
      } 
      else if(strcmp(command,"wait") == 0) { // buzzer
        int timer;
        sscanf(curCode + (*curCodePos), "%d", &timer);
        (*curCodePos) += floor(log10(abs(timer))) + 1;
        delay(timer);   
      }
      else if(strcmp(command,"move") == 0) { // buzzer
        if(curCode[(*curCodePos)++] == '0')
          setServo(-200, -200, 20);
        else
          setServo(200, 200, 20);
      }
      else if(strcmp(command,"turn") == 0) { // buzzer
        if(curCode[(*curCodePos)++] == '0')
          setServo(-200, 200, 1500);
        else
          setServo(200, -200, 1500);
         setServo(0,0,20);
      }
      else if(strcmp(command,"stop") == 0) {
        setServo(0,0,20);
      }
      
      return 0;
  }
  else if (*curLoopPos >= 0) {
    (*curCodePos) = *curLoopPos; 
    return 0;
  }
  
  return -1; // program ended
}

void resetState()
{
  receivingCode = true;
  recvReact = false;
  count = 0;
  reactCount = 0;
  code_pos = 0;
  reactCodePos = 0;
  loopPos = -1;
  reactLoopPos = -1;
  isReacting = false;
}

void loop() {
  char codeChar;
  int sensorLeft = sensorDistance(SENSOR_ONE_OUT, SENSOR_ONE_IN);            
  int sensorRight = sensorDistance(SENSOR_TWO_OUT, SENSOR_TWO_IN);  
  
  // TODO: Fix blue sensor
  if(millis() > initial + 15000) {
    isReacting = true;
    Serial.println("DETECTED");
  }
    
  if (bluetooth.available()) {
    if(!receivingCode)
      resetState();
      
    codeChar = (char)bluetooth.read();
    if (codeChar == '/')
      receivingCode = false;  
    else if (codeChar == '|')
      recvReact = true;
    else if (!recvReact)
      code[count++] = codeChar;
    else
      reactCode[reactCount++] = codeChar;
  }

  if (millis() > last_show + 1000) {
     Serial.print("code: ");
     Serial.println(code);
     Serial.print("react: ");
     Serial.println(reactCode);
     last_show = millis();
  }
  
  int ret;
  if (!receivingCode)
    ret = runCode();
    
  // code has finished, so receive new code
  if (ret == -1) {
    if (isReacting) {
      isReacting = false;
      initial = millis();
    }
    else
      resetState();
  }
//  if (Serial.available()) {
//    bluetooth.write(Serial.read());
//  }
}


