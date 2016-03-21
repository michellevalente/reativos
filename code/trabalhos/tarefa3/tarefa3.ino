#include "event_driven.h"
#define LED_PIN 13
#define BUTTON_DECELERATE 2
#define BUTTON_ACELERATE 3

unsigned long velocity;
int led_state;

void my_init(){
  button_listen(BUTTON_DECELERATE);
  button_listen(BUTTON_ACELERATE);
  velocity = 500;
  timer_set(velocity);
  led_state = LOW;
  pinMode(LED_PIN, OUTPUT);
}

void button_changed(int pin, int state){
  if(state == HIGH )
  {
    if(pin == BUTTON_ACELERATE)
      velocity = velocity * 2;
    else
      velocity = velocity / 2;
  }
}

void timer_expired(){
  digitalWrite(LED_PIN, led_state);
  led_state = !led_state;
  timer_set(velocity);
}
