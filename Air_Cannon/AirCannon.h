#ifndef AIRCANNON_H
#define AIRCANNON_H
#include <Arduino.h>
#include <Servo.h>
#include "debouncer.h"

class airCannon{
  private:
  enum STATE{IDLE_STATE, DRAWING};
  STATE robotState = IDLE_STATE;
  int servoPin;
  db trigger(int);
  db endstop(int);

  public:
  airCannon(int trigPin, int esPin, int sp);
  void Init();
  void fire();
};

#endif
