#define LED_PIN 13
#define BTN1_PIN 2
#define BTN2_PIN 4
 
unsigned long time, btn1Time, btn2Time, interval, changeTime;
int ledState, btn1State, btn2State;
 
void setup() {
  ledState = btn1State = btn2State = 0;
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN1_PIN, INPUT);
  pinMode(BTN2_PIN, INPUT);
  time = 0;
  btn1Time = btn2Time = 0;
  interval = 1000; 
  changeTime = 0;
}
 
void loop() {
  unsigned long t = millis();
  if(t >= time + interval){
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    time = t;
  }
 
  int btn1s = digitalRead(BTN1_PIN);
  int btn2s = digitalRead(BTN2_PIN);
 
  if(btn1s != btn1State && t - changeTime > 200){
    interval += 100;
    btn1State = btn1s;
    btn1Time = t;
    changeTime = millis();
  }
 
  if(btn2s != btn2State && t - changeTime > 200){
    interval -= 100;
    if(interval < 0) interval = 0;
    btn2State = btn2s;
    btn2Time = t;
    changeTime = millis();
  }
 
  if(btn1s && btn2s && (btn1Time-btn2Time <= 500 || btn2Time - btn1Time <= 500)){
    digitalWrite(LED_PIN, LOW);
    while(1);
  }
 
}
