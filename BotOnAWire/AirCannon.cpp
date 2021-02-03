#include "AirCannon.h"

airCannon::airCannon(db _endstop, int sp): endstop(_endstop){
  servoPin = sp;
}

void airCannon::Init(){
  //trigger.Init();
  endstop.Init();
  pinion.attach(servoPin);
  pinion.write(89);
}

boolean airCannon::Fire(){
  draw();
  return robotState == DRAWING; //done drawing
}

boolean airCannon::DoneFire(){
  if(endstop.checkButtonPress()) hold(); 
  return  robotState == IDLE_STATE; 
}

void airCannon::draw(){
  switch(robotState){
    case IDLE_STATE:
      pinion.write(45);
      Serial.println("drawing pinion");
      robotState = DRAWING;
      break;
  }
}

void airCannon::hold(){
  switch(robotState){
    case DRAWING:
      pinion.write(89);
      robotState = IDLE_STATE;
      Serial.println("DONE FIRE");
      done = true;
      break;
  }
}
