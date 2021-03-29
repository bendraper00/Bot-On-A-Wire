//#include <HCSR04.h>

#include <ESC.h>
//#include <ArduinoJson.h>
//#include <Ultrasonic.h>
//#include "AirCannon.h"
//#include "debouncer.h"
//#include "DirectionalSound.h"
//#include <Servo.h>

#define USPin1 A0  //front
#define USPin2 A2    //back
#define arrayLength 10
#define cannon_endPin 2 //change if needed
#define cannon_pinionPin 3 //change if needed

ESC myESC1 (8, 1000, 2000, 2000);
ESC myESC2 (9, 1000, 2000, 2000);
//db cannonEnd (cannon_endPin);
//Servo pinion;
//airCannon* Cannon = new airCannon(cannonEnd, cannon_pinionPin);
//DirectionalSound dirSound;

float forwardDistances[arrayLength];
bool forward = true;
int motorSpeed = 1500;
bool dir_forward = true;
int stopSpeed = 1500;
double stopDistance = 30;
int speedRange = 110;  //+- from 1500
int speedSafety = 50;
int horRange = 640;
double distanceRange = 50;
float frontDist =0;
 float backDist =0;

struct DetectObject{
  double area;
  double x;
  double y; 
  };

enum RobotState {DETECT, LOOKING};
enum CannonState {DRAWING, HOLDING};
RobotState state = LOOKING;
CannonState cannon_st = DRAWING;

void setup() {
  Serial.begin(19200);
  Serial.setTimeout(10000);
  Serial.println("Hello");
  Serial.end();
  //Cannon->Init();
  //pinion.attach(cannon_pinionPin);
  //pinion.write(89);
  //dirSound.init();
  myESC1.arm();
  myESC2.arm();
  myESC1.speed(1500);
  myESC2.speed(1500);
  delay(5000);
  pinMode(13, OUTPUT);
  Serial.begin(19200);
}

void loop() {
  frontDist = getUltrasonicDistance(true);  //front == true
  backDist = getUltrasonicDistance(false);
  //Serial.print(frontDist);
  //Serial.print (" ");
  //Serial.println(backDist);
  addToArray(frontDist);
  
   if (Serial.available() > 0) {
    motorSpeed = ReadParseSerial();
   }
 /* if (forward && frontDist <= stopDistance || (!forward && backDist <= stopDistance)) { //if too close
      motorSpeed = 1500;
  }*/
   myESC1.speed(motorSpeed);
   myESC2.speed(motorSpeed);

  //CannonControl();
  //dirSound.update();
}

/*void CannonControl()
{
  switch (cannon_st)
  {
    case DRAWING:
    if (state == DETECT)
    {
      pinion.write(45);
      Serial.println("drawing pinion");
      cannon_st = HOLDING;
    }
    break;
    case HOLDING:
      if (cannonEnd.checkButtonPress()) 
      {
        pinion.write(89);
      cannon_st = DRAWING;
      Serial.println("DONE FIRE");
      }
    break;
    }
}*/

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

int ReadParseSerial()
{
    String sRead= Serial.readStringUntil('\n');
    String str;
    DetectObject detectArray[15];
    //Serial.println(sRead);
       
    int i =0;
    int j =0;
    if (sRead.length() > 0)
    {
    while (i < sRead.length())
    {
      //DetectObject obj;
      int pos = sRead.indexOf(",", i);
      if (pos == -1 && i<sRead.length())
      {
        str = sRead.substring(i);
        i = sRead.length();
      }
      else
      {
      str = sRead.substring(i, pos);
      i = pos+1;
      }
    
    String part01 = getValue(str,' ',0);
    String part02 = getValue(str,' ',1);
    String part03 = getValue(str,' ',2);
     DetectObject obj = {part01.toDouble(), part02.toDouble(), part03.toDouble()};
     detectArray[j] = obj;
      j++;
      if (part01 == "0")
      {
        state = LOOKING;
        return 1500;
      }
    }
    state = DETECT;
    Serial.println ("detect");
    return DetectControl(detectArray, j);
    }
    
    state = LOOKING;
    return 1500;
  
}
/**
 * For now this function will get the max area, calculate angle of camera to the closest bird
 * then determine movement left or right
 */
int DetectControl(DetectObject detectArray[],int arraySize)
{
  DetectObject closestOne = {0,0,0};
  for (int i =0; i< arraySize; i++)
  {
    if (detectArray[i].area > closestOne.area)
    {
      closestOne = detectArray[i];
    }
  } 
  return CalcDirection(closestOne.x, closestOne.y); 
}

double CalcDirection (double x, double y)
{
  double angleToX =0;
  double centerX = 640;
  double centerY = 360;
  double angle =0;  //do nothing for now
  bool isUp = false;
  bool isFront = false;

  double thetaX = x - centerX;
  double thetaY = y - centerY;
  //angle = Math.atan2(thetaY/thetaX);
  if (thetaX < 0) 
  {
    isFront = true;
    //Serial.println ("LEFT");
    }
    else
    {
      //Serial.println ("RIGHT");
    }
  if (thetaY < 0) 
  {
    isUp = true;
  }

   if (isFront)
   {
    forward = true;
    return (CalcSpeed_demo (frontDist, thetaX));
   }
   else
   {
    forward = false;
   return (CalcSpeed_demo (backDist, thetaX));
   }
}

/**
 * Calculat ethe speed if not close to pole -> go left or right if needed
 */
int CalcSpeed_demo (float distance,double thetaX)
{
  int mySpeed =0;

  if (distance <= stopDistance )
  {
    mySpeed = 1500;
  }  
  else
  {
    mySpeed = stopSpeed + (thetaX * speedRange)/horRange;

    if (mySpeed < stopSpeed - speedRange) 
    {
      mySpeed = stopSpeed - speedRange;
    }
    else if (mySpeed > stopSpeed + speedRange) 
    {
      mySpeed = stopSpeed + speedRange;
    }

    if (mySpeed > stopSpeed - speedSafety && mySpeed < stopSpeed) mySpeed = stopSpeed;
    else if (mySpeed < stopSpeed + speedSafety && mySpeed > stopSpeed) mySpeed = stopSpeed;
  }
  return mySpeed;
}


float getUltrasonicDistance(bool isFront) // returns distance in centimeters
{
  int distance = 0;
  if (isFront) distance = analogRead(USPin1);
  else distance = analogRead(USPin2);

  return (distance/1024.0)*512*2.54;
  //return 50;
}

void addToArray(float dist)
{
  for (int k = arrayLength - 1; k > 0; k--)
  {
    float old = forwardDistances[k - 1];
    forwardDistances[k] = old;
    //Serial.print(old);
  }
  forwardDistances[0] = dist;
  //Serial.println(forwardDistances[0]);
}

bool tooClose(float dist, int valid)
{
  int count = 0;
  for (int k = 0; k < arrayLength - 1; k++)
  {
    if (forwardDistances[k] < dist) count++;
  }
  //Serial.println(count);
  return count >= valid;
}

float frontAvg()
{
  int count = 0;
  for (int k = 0; k < arrayLength - 1; k++)
  {
    count += forwardDistances[k];
  }
  float avg = count / arrayLength;
  //Serial.println(avg);
  return avg;
}
