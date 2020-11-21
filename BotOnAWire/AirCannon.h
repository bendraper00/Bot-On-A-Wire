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
  int trigPin;
  int esPin;
  db trigger;
  db endstop;
  Servo pinion;
  boolean done = false;

  public:
  airCannon(db trigPin, db esPin, int sp);
  void Init();
  void Fire();
  boolean DoneFire();

  protected:
  void draw();
  void hold();
};

#endif
