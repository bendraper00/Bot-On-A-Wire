#include "AirCannon.h"

airCannon::airCannon(int trigPin, int esPin, int sp){
  trigger = db trigger(trigPin);
  endstop = db endstop(esPin);
  servoPin = sp;
}

airCannon::Init(){
  trigger.Init();
  endstop.Init();
  pinion.attach(servoPin);
  pinion.write(89);
}
