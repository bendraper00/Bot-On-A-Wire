#include <Servo.h>
#include "debouncer.h"
#include <EventTimer.h>

Servo pinion;

enum STATE{IDLE_STATE, DRAWING};
STATE robotState = IDLE_STATE;
db trigger(2);
db endstop(13);

int checker1 = 0;
int checker2 = 0;

void setup() {
  Serial.begin(9600);
  trigger.Init();
  endstop.Init();
  pinion.attach(9);
}

void loop() {
  //if(trigger.checkButtonPress()) checker1++;
  if(endstop.checkButtonPress()) checker2++;
  Serial.println(String(checker1) + '\t' + String(checker2));
  
  //if(trigger.checkButtonPress()) draw();
  //if(endstop.checkButtonPress()) hold();
}

void draw(){
  switch(robotState){
    case IDLE_STATE:
      pinion.write(135);
      robotState = DRAWING;
      break;
  }
}

void hold(){
  switch(robotState){
    case DRAWING:
      pinion.write(90);
      robotState = IDLE_STATE;
  }
}
