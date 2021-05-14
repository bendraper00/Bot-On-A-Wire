#include "Gimbal.h"

void Gimbal::init(){
    pinMode(YENCA, INPUT);
    pinMode(YENCB, INPUT);
    pinMode(PENCA, INPUT);
    pinMode(PENCB, INPUT);
    attachInterrupt(digitalPinToInterrupt(YENCA), isrY, CHANGE);
    attachInterrupt(digitalPinToInterrupt(YENCB), isrY, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PENCA), isrP, CHANGE);
    attachInterrupt(digitalPinToInterrupt(PENCB), isrP, CHANGE);
    pinMode(LIMITPITCH, INPUT_PULLUP);
    pinMode(LIMITYAW, INPUT_PULLUP);
    this.home();
}
void Gimbal::home(){
    while(digitalRead(LIMITPITCH) != LOW){
    pitchMotor.drive(200);
    }
    pitchMotor.drive(0);
    pitchCount = 0;
    while(digitalRead(LIMITYAW) != LOW){
        yawMotor.drive(-180);
    }
    yawMotor.drive(0);
    yawCount = 0;
}
void Gimbal::PIyaw(){
  const double P = 100, I = 5;
  static double cError = 0;
  if( Yangle < 0 || Yangle > 225 || digitalRead(LIMITPITCH) == LOW){
    yawMotor.drive(0);
    return;
  }
  double error = Yangle - ((double)yawCount)/YCPR*360;
  if(Yangle == 0 && error < 1 && digitalRead(LIMITPITCH) != LOW){
      yawCount += YCPR*360/0.1;
  }
  cError += error;
  yawMotor.drive(constrain(error*P+cError*I,-255,255));
  
}
void Gimbal::PIpitch(){
  const double P = 100, I = 5;
  static double cError = 0;
  if( Pangle < 0 || Pangle > 90 || digitalRead(LIMITYAW) == LOW)){
    pitchMotor.drive(0);
    return;
  }
  double error = Pangle -((double)pitchCount)/PCPR*360;
    if(Pangle == 0 && error < 1 && digitalRead(LIMITYAW) != LOW){
      pitchCount += PCPR*360/0.1;
  }
  cError += error;
  pitchMotor.drive(constrain(-1*(error*P+cError*I),-255,255));
}
void Gimbal::update(){
    this.PIpitch();
    this.PIyaw();
    if(digitalRead(LIMITYAW) == LOW){
        yawCount = 0;
    }
    if(digitalRead(LIMITPITCH) == LOW){
        pitchCount = 0;
    }
}
void Gimbal::setYawAngle(double angle){
    Yangle = angle;
}
void Gimbal::setPitchAngle(double angle){
    Pangle = angle;
}
double Gimbal::getYawAngle(){
    return ((double)yawCount)/YCPR*360;
}
double Gimbal::getPitchAngle(){
    return ((double)pitchCount)/PCPR*360;
}

void isrY(){
  int newValue = (digitalRead(YENCB)<<1) | digitalRead(YENCA);
  char value = encoderArray[oldValueY][newValue];
  if(value != X){
    yawCount+= value;
  }
  oldValueY = newValue;
}
void isrP(){
  int newValue = (digitalRead(PENCB)<<1) | digitalRead(PENCA);
  char value = encoderArray[oldValueP][newValue];
  if(value != X){
    pitchCount+= value;
  }
  oldValueP = newValue;
}