//#include <HCSR04.h>

#include <ESC.h>
#include <ArduinoJson.h>
#include <Ultrasonic.h>
#include "AirCannon.h"
#include "debouncer.h"

#define IRPin A0
#define trigPin 8
#define echoPin 9
#define arrayLength 10
#define cannon_trigPin 2 //change if needed
#define cannon_endPin 13 //change if needed
#define cannon_pinionPin 9 //change if needed

ESC myESC (11, 1000, 2000, 2000);
db cannonTrig (cannon_trigPin);
db cannonEnd (cannon_endPin);
airCannon Cannon(cannonTrig, cannonEnd, cannon_pinionPin);
//Ultrasonic ultra (trigPin, echoPin);

float forwardDistances[arrayLength];

bool forward = true;
int motorSpeed = 1500;

bool dir_forward = true;
int stopSpeed = 1500;
double stopDistance = 30;
int speedRange = 100;
double distanceRange = 50;
bool doneFire= true;

struct DetectObject{
  double area;
  double x;
  double y; 
  };

void setup() {
  Serial.begin(19200);
  Serial.setTimeout(10000);
  Serial.println("Hello");
  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);
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
}


  
void loop() {
  //{"speed":1500}
  float frontDist = getIRDist();
  float backDist = getUltrasonicDistance();
  //float backDist = ultra.read(); //Pass INC as parameter for dist in inch
  addToArray(frontDist);
  //Serial.println ("in loop");
  
   if (Serial.available() > 0) {
    Serial.println("reading");
    //{"speed": 1500}
    String json = Serial.readStringUntil('\n');
    int income = Serial.read();
    StaticJsonDocument<800> docIn;
    DeserializationError error = deserializeJson(docIn, json);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    else
    {
     Serial.print ("no Error");
     Serial.print (json); 
     Serial.print(sizeof(docIn));
     }
    
    JsonArray input = docIn["detections"].to<JsonArray>();
    //motorSpeed = docIn["speed"];
    //Serial.println("Motor Speed: " + String(motorSpeed));
    
    if (input.size() > 0)
    {
    DetectObject detectArray[15];
    Serial.print ("about ot read input");
    for(int i =0; i < 15; i++)
    {
      DetectObject obj = {0,0,0};
      if (i < input.size())
      { 
      obj.area = input[i]["area"];
      Serial.print (obj.area);
      obj.x = input[i]["xCord"];
      Serial.print (" ");
      Serial.print (obj.x);
      obj.y = input[i]["yCord"];
      Serial.print (" ");
      Serial.print (obj.y);
        
      }
      detectArray[i] = obj;
    }
    motorSpeed = DetectControl(detectArray);
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level) 
    
    }
    else
    {
      Serial.println("empty input");
      motorSpeed = 1500;
     digitalWrite(13, LOW); 
    }
    
    StaticJsonDocument<200> doc;
    serializeJson(doc, Serial);
    Serial.println("");
    }
    else
    {//Serial.println ("serial not available");
      }
    //Serial.println(getIRDist());
    
  if (forward) {
    //Serial.println("forward");
    if (tooClose(20, 9) && motorSpeed > 1500) {
      myESC.speed(1500);
    }
    else {
      myESC.speed(motorSpeed);
      //Serial.println(motorSpeed);
    }
  }
  else {
    if (backDist > 80) backDist = 80;
    if (backDist < 40) {
      myESC.speed(1600);
      forward = true;
    }
  }  
  Serial.println(motorSpeed);
}

/**
 * For now this function will get the max area, calculate angle of camera to the closest bird
 * then determine movement left or right
 */
int DetectControl(DetectObject detectArray[])
{
  DetectObject closestOne = {0,0,0};
  for (int i =0; i< sizeof(detectArray)/sizeof(detectArray[0]); i++)
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
  double centerX = 540;
  double centerY = 360;
  double angle =0;  //do nothing for now
  bool isUp = false;
  bool isLeft = false;

  double thetaX = x - centerX;
  double thetaY = y - centerY;
  //angle = Math.atan2(thetaY/thetaX);
  if (thetaX < 0) isLeft = true;
  if (thetaY < 0) isUp = true;

   if (isLeft) return (CalcSpeed_demo (getUltrasonicDistance(), true));
   else return (CalcSpeed_demo (getIRDist(), false));
}

/**
 * Calculat ethe speed if not close to pole -> go left or right if needed
 */
int CalcSpeed_demo (float distance, boolean isLeft)
{
  int mySpeed =0;
  if (isLeft) forward = false;
  else forward = true;

  if (distance <= stopDistance)
  {
    mySpeed = 1500;
  }  
  else
  {
    if (isLeft) mySpeed = 1400;
    else mySpeed = 1600;
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

float getUltrasonicDistance()
{
  return 5;
  /*long duration;
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    // Trigger the sensor by setting the trigPin high for 10 microseconds:
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Read the echoPin, pulseIn() returns the duration (length of the pulse) in microseconds:
    duration = pulseIn(echoPin, HIGH);
    // Calculate the distance:
    return duration * 0.034 / 2;
  */
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
