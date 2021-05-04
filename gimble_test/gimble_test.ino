#include <SparkFun_TB6612.h>

#define YENCA 2
#define YENCB 3
#define PENCA 0
#define PENCB 1
#define LIMITPITCH 4
#define LIMITYAW 11
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
  Serial.begin(9600);
}
int timeOld = millis();
double target = 0;
bool inc = true;
void loop() {
  /*if(millis() - timeOld > 50){
    target += inc?0.1:-0.1;
    timeOld = millis();
  }
  if(target > 225 || target < 0) inc = !inc;
  PIyaw(target);*/
  Serial.print(target);
  Serial.print(" ");
  Serial.println(((double)yawCount)/YCPR*360);
    
}
void PIyaw(double angle){
  const double P = 100, I = 0;
  static double cError = 0;
  if( angle < 0 || angle > 225)
    return;
  double error = ((double)yawCount)/YCPR*360 - angle
  ;
  cError += error;
  yawMotor.drive(constrain(error*P+cError*I,-255,255));
}
void PIpitch(double angle){
  const double P = 100, I = 0;
  static double cError = 0;
  if( angle < 0 || angle > 225)
    return;
  double error = ((double)pitchCount)/PCPR*360 - angle;
  cError += error;
  pitchMotor.drive(constrain(error*P+cError*I,-255,255));
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
