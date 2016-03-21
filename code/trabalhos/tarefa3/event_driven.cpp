#include "Arduino.h"
#include "event_driven.h"

#define MAX_BUTTONS 2

int ButtonPin[MAX_BUTTONS];
int ButtonState[MAX_BUTTONS];
int NumPins = 0;
int t;

/* Funcoes de registro */

void button_listen(int pin){
   if(NumPins < MAX_BUTTONS)
   {
      ButtonPin[NumPins++] = pin;
      pinMode(pin, INPUT);
   }
}

void timer_set(int ms){
   if(t == -1)
     t = millis() + ms;
}

/* Programa Principal */

void setup() {
  int i;
  for(i=0; i< MAX_BUTTONS; i++)
  {
    ButtonState[i] = 0;
    ButtonPin[i] = 0;
  }  
  t = -1;
  my_init();
}

void loop() {
    int state_now, i;

    if(millis() >= t)
    {
      t = -1;  
      timer_expired();
    }

    for(i=0; i< NumPins; i++)
    {
       state_now = digitalRead(ButtonPin[i]);
       if(ButtonState[i] != state_now){
         ButtonState[i] = state_now;
         button_changed(ButtonPin[i], state_now);
       } 
    }
}
