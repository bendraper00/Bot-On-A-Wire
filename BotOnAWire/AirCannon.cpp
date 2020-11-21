#include "AirCannon.h"

airCannon::airCannon(db _trigger, db _endstop, int sp): trigger(_trigger), endstop(_endstop){
  //trigger = db trigger(trigPin);
  //endstop = db endstop(esPin);
  servoPin = sp;
}

void airCannon::Init(){
  trigger.Init();
  endstop.Init();
  pinion.attach(servoPin);
  pinion.write(89);
}

void airCannon::Fire(){
  if(trigger.checkButtonPress()) draw();
}

boolean airCannon::DoneFire(){
  if(endstop.checkButtonPress()) hold(); 
  return robotState == IDLE_STATE; 
}

void airCannon::draw(){
  switch(robotState){
    case IDLE_STATE:
      pinion.write(45);
      robotState = DRAWING;
      break;
  }
}

void airCannon::hold(){
  switch(robotState){
    case DRAWING:
      pinion.write(89);
      robotState = IDLE_STATE;
      done = true;
  }
}
