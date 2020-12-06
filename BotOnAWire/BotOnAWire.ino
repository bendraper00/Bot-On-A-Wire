//#include <HCSR04.h>

#include <ESC.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include "AirCannon.h"
#include "debouncer.h"

#define IRPin A0
#define USPin A2
#define arrayLength 10
#define cannon_trigPin 2 //change if needed
#define cannon_endPin 13 //change if needed
#define cannon_pinionPin 9 //change if needed

ESC myESC (11, 1000, 2000, 2000);
db cannonTrig (cannon_trigPin);
db cannonEnd (cannon_endPin);
airCannon Cannon(cannonTrig, cannonEnd, cannon_pinionPin);

float forwardDistances[arrayLength];

bool forward = true;
int motorSpeed = 1500;

bool dir_forward = true;
int stopSpeed = 1500;
double stopDistance = 30;
int speedRange = 100;
int horRange = 540;
double distanceRange = 50;
bool doneFire= true;
float frontDist =0;
 float backDist =0;

struct DetectObject{
  double area;
  double x;
  double y; 
  };

void setup() {
  Serial.begin(19200);
  Serial.setTimeout(10000);
  Serial.println("Hello");
  Serial.end();
  //Cannon.Init();
  delay(15000);
  myESC.arm();
  delay(15000);
  myESC.speed(2000);
  delay(500);
  myESC.speed(1000);
  delay(500);
  myESC.speed(1500);
  delay(500);
  pinMode(13, OUTPUT);
  Serial.begin(19200);
}


  
void loop() {
  frontDist = getIRDist();
  backDist = getUltrasonicDistance();
  //Serial.println (getIRDist());
  //Serial.print (" ");
  //Serial.println (getUltrasonicDistance());
  addToArray(frontDist);
  
   if (Serial.available() > 0) {
    motorSpeed = ReadParseSerial();
   }
    
  if (forward && frontDist <= stopDistance) {
      motorSpeed = 1500;
      Serial.println("Too Close IR");
  }
  else if (!forward && backDist <= stopDistance ) {
      motorSpeed = 1500;
      Serial.println("Too Close Ultra");
  }
  
   myESC.speed(motorSpeed); 
  //Serial.println(motorSpeed);
}


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
    Serial.println(sRead);
       
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
        return 1500;
      }
    }
    
    return DetectControl(detectArray, j);
    }
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
    Serial.println ("LEFT");
    }
    else
    {
      Serial.println ("RIGHT");
    }
  if (thetaY < 0) 
  {
    isUp = true;
  }

   if (isFront)
   {
    return (CalcSpeed_demo (frontDist, isFront, thetaX));
   }
   else
   {
   return (CalcSpeed_demo (backDist, isFront, thetaX));
   }
}

void testDirection()
{
  CalcDirection (500, 0);
  CalcDirection (600, 0);
  CalcDirection (400 , 0);
  CalcDirection (1000, 0);
}
/**
 * Calculat ethe speed if not close to pole -> go left or right if needed
 */
int CalcSpeed_demo (float distance, boolean isFront, double thetaX)
{
  int mySpeed =0;
  if (isFront) forward = true;
  else forward = false;

  if (distance <= stopDistance )
  {
    //Serial.println (distance);
    mySpeed = 1500;
  }  
  else
  {
    mySpeed = stopSpeed - (thetaX * speedRange)/horRange;
    if (mySpeed < stopSpeed - speedRange) 
    {
      mySpeed = stopSpeed - speedRange;
    }
    else if (mySpeed > stopSpeed + speedRange) 
    {
      mySpeed = stopSpeed + speedRange;
    }
  }
  return mySpeed;
}


///Only apply for the front now
int CalcSpeed(float distance, boolean isLeft)
{ 
  //isleft = true =  backward
  //isLeft = false = forward
  int my_speed = 0;
  if (distance < stopDistance and forward)
  {
    my_speed = 1400;
    dir_forward = false;
  }
  else if (distance < distanceRange and not forward)
  {
    my_speed = 1450;
  }
  else if (distance > distanceRange - 10)
  {
    dir_forward = true;
    my_speed = 1500;
  }
  else {
    dir_forward = true;
    my_speed = stopSpeed + speedRange - (distanceRange - distance) / distanceRange * speedRange;
    if (my_speed < stopSpeed - speedRange) 
    {
      my_speed = stopSpeed - speedRange;
    }
    else if (my_speed > stopSpeed + speedRange) 
    {
      my_speed = stopSpeed + speedRange;
    }
  }
  Serial.print(distance);
  Serial.print(" ");
  Serial.print(my_speed);
  return my_speed;
}

float getIRDist()
{
  float IRDist = 0;
  float IRVal = analogRead(IRPin);
  float IRVolt = (1024 / (IRVal * 5 ));
  IRDist = (IRVolt - 0.0512) * 24.5;
  if (IRDist > 80) IRDist = 80;
  return IRDist;
}

float getUltrasonicDistance() // returns distance in centimeters
{
  //return (analogRead(USPin)/1024.0)*512*2.54;
  return 50;
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
