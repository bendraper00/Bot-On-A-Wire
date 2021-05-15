#include <SparkFun_TB6612.h>

#define YENCA 2
#define YENCB 3
#define PENCA 0
#define PENCB 1
#define LIMITPITCH 11
#define LIMITYAW 4
#define AIN1 13
#define BIN1 8
#define AIN2 6
#define BIN2 9
#define PWMA 5
#define PWMB 10
#define STBY 12
#define YCPR 1080 //yaw encoder counts per revolution (of end effector)
#define PCPR 17310 //very back of the envolope pitch encoder counts per revolution (of end effector)
const char X = 5;
long yawCount = -1;
long pitchCount = -1;
const char encoderArray[4][4] = {
    {0,-1,1,X},
    {1,0,X,-1},
    {-1,X,0,1},
    {X,1,-1,0}
};
int oldValueP =0;
int oldValueY =0;
const int offsetA = 1;
const int offsetB = 1;

Motor yawMotor = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor pitchMotor = Motor(BIN1, BIN2, PWMB, offsetB, STBY);
void setup() {
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
  //Serial.begin(9600);
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
unsigned long timeOld = millis();
double targetY = 0, targetP = 0;
bool inc = true;
double cycleTime = 4;
double dpsYaw = 225/cycleTime;
double dpsPitch = 90/cycleTime;
void loop() {
  if (millis() - timeOld < cycleTime*1000){
    targetY = dpsYaw*(millis() - timeOld)/1000.0;
    targetP = dpsPitch*(millis() - timeOld)/1000.0;
  }else if(millis() - timeOld < cycleTime*2000){
    targetY = 225 - dpsYaw*(millis() - (timeOld + cycleTime*1000))/1000.0;
    targetP = 90 - dpsPitch*(millis() - (timeOld + cycleTime*1000))/1000.0;
  }else{
    timeOld = millis();
  }
  PIyaw(targetY);
  PIpitch(targetP);
}
void PIyaw(double angle){
  const double P = 100, I = 0;
  static double cError = 0;
  if( angle < 0 || angle > 225){
    yawMotor.drive(0);
    return;
  }
  double error = angle - ((double)yawCount)/YCPR*360;
  cError += error;
  yawMotor.drive(constrain(error*P+cError*I,-255,255));
}
void PIpitch(double angle){
  const double P = 100, I = 0;
  static double cError = 0;
  if( angle < 0 || angle > 90){
    pitchMotor.drive(0);
    return;
  }
  double error = angle -((double)pitchCount)/PCPR*360;
  cError += error;
  pitchMotor.drive(constrain(-1*(error*P+cError*I),-255,255));
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
