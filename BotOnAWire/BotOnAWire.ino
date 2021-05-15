#include <ESC.h>
#include <SoftwareSerial.h>
#include "Lights.h"
#include "DirectionalSound.h"
#include "Gimbal.h"
#include "MedianFilter.h"

#define USPin1 A0  //front
#define USPin2 A2    //back
#define arrayLength 10

ESC myESC1 (8, 1000, 2000, 2000);
ESC myESC2 (9, 1000, 2000, 2000);


DirectionalSound dirSound;
Lights strobe;
Gimbal gimbal;

float getUltrasonicDistance(bool isFront) ;
int ReadParseSerial();

int motorSpeed = 1500;
int stopSpeed = 1500;
double stopDistance = 30;
int speedRange = 130;  //+- from 1500
int patrollingSpeed = 80;
int speedSafety = 50;
int horRange = 640;
double distanceRange = 50;

bool webcam = true;
unsigned long lastDetect = 0;
float forwardDistances[arrayLength];
bool dir_forward = true;

unsigned int lastInput = 0;
MedianFilter frontDist;
MedianFilter backDist;

struct DetectObject {
  double area;
  double x;
  double y;
};

enum RobotState {DETECT, LOOKING, GOHOME, CHARGING};
//enum CannonState {DRAWING, HOLDING};
RobotState state = LOOKING;

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(10000);
  Serial.println("Hello");
  Serial.end();

  dirSound.init();
  myESC1.arm();
  myESC2.arm();
  myESC1.speed(1500);
  myESC2.speed(1500);
  delay(5000);

  Serial.begin(19200);
  strobe.init();
  dirSound.init();
  gimbal.init();
  strobe.on();
}


void loop() {
  strobe.flash();
  dirSound.update();
  frontDist.push(getUltrasonicDistance(true) +8 ); //front == true// front Dist tends to read high
  backDist.push( getUltrasonicDistance(false) + 6); //back dist tends to read low

//  if(voltage > 0 && voltage < 9.7){
//    state = GOHOME;
//  }

  if (Serial.available() > 0) {
    ////Serial.println("detect\n");
    
    //Serial.print("go ");
    //Serial.println(webcam);
    
    motorSpeed = ReadParseSerial(); // reading input from jetson
    
  }
  
  if (state == LOOKING && millis() - lastDetect > 1000) { // if not currently chasing
    //webcam = !webcam;
    strobe.off();
    if (frontDist.read() <= stopDistance && backDist.read() <= stopDistance ) {
      //Serial.print("Stopped");
      motorSpeed = stopSpeed;
    } else {
      if ( frontDist.read() <= stopDistance ) {
        //if too close
        //Serial.print("BACKWARD");
        dir_forward = false;
      } else if ( backDist.read() <= stopDistance) {
        //Serial.print("FORWARD");
        dir_forward = true;
      } if (dir_forward) {
        motorSpeed = stopSpeed + patrollingSpeed;
      } else if (!dir_forward) {
        motorSpeed = stopSpeed - patrollingSpeed;
      }
    }
    lastInput = millis();
  }else if (state == GOHOME){
    strobe.off();
    if(!backDist.read() >= stopDistance){
      motorSpeed = stopSpeed + patrollingSpeed;
      dir_forward = false;
      lastInput = millis();
    }else{
      if(lastInput- millis() < 2000){ // just continue moving towards the docking station for 2 secs untill we have docking station detection
        motorSpeed = stopSpeed - patrollingSpeed*0.75;
      }else{
        motorSpeed = stopSpeed;
        dir_forward = true;
        state = CHARGING;
        lastInput = millis();
      }
    }
  }else if (state == CHARGING){
    strobe.off();
    if(lastInput - millis() < 10000){// wait 10 seconds after docking since we aren't really charging yet
      state = LOOKING;
    }
  }
  if(state == DETECT){
    strobe.on();
  }
    
  //  Serial.println(state);
  //Serial.println(motorSpeed);
    myESC1.speed(motorSpeed);
    myESC2.speed(motorSpeed);

  strobe.flash();
  //dirSound.update();
}


String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}


int ReadParseSerial()
{
  String sRead = Serial.readStringUntil('\n');
  String str;
  DetectObject detectArray[15];
  //Serial.println(sRead);

  int i = 0;
  int j = 0;
  if (sRead.length() > 0) {
    while (i < sRead.length()) {
      //DetectObject obj;
      int pos = sRead.indexOf(",", i);
      if (pos == -1 && i < sRead.length()) {
        str = sRead.substring(i);
        i = sRead.length();
      } else {
        str = sRead.substring(i, pos);
        i = pos + 1;
      }

      String part01 = getValue(str, ' ', 0);
      String part02 = getValue(str, ' ', 1);
      String part03 = getValue(str, ' ', 2);
      DetectObject obj = {part01.toDouble(), part02.toDouble(), part03.toDouble()};
      detectArray[j] = obj;
      j++;
      if (part01 == "0") {
        state = LOOKING;
        return motorSpeed;
      }
    }
    state = DETECT;
    lastDetect = millis();
    return DetectControl(detectArray, j);
  }

  state = LOOKING;
  return motorSpeed;

}
/**
   For now this function will get the max area, calculate angle of camera to the closest bird
   then determine movement left or right
*/
int DetectControl(DetectObject detectArray[], int arraySize)
{
  DetectObject closestOne = {0, 0, 0};
  for (int i = 0; i < arraySize; i++)
  {
    if (detectArray[i].area > closestOne.area)
    {
      closestOne = detectArray[i];
    }
  }
  return CalcDirection(closestOne.x, closestOne.y);
}

/*
  Calculate the location of the detection with respect to the frame
  and return the the speed calculated from the given pixel coordinate detected
*/
double CalcDirection (double x, double y)
{
  double angleToX = 0;
  double centerX = 640;
  double centerY = 360;
  double angle = 0; //do nothing for now
  bool isUp = false;
  bool isFront = true;

  double thetaX = x - centerX;
  double thetaY = y - centerY;
  //angle = Math.atan2(thetaY/thetaX);
  if(!webcam){
    thetaX = -1*thetaX;
  }
  if (thetaX < 0) {
    isFront = false;
    //Serial.println ("FORWARD");
  }
  if (thetaY < 0) {
    isUp = true;
  }
  if (isFront) {
    dir_forward = true;
    return (CalcSpeed_demo (frontDist.read(), thetaX));
  } else {
    dir_forward = false;
    return (CalcSpeed_demo (backDist.read(), thetaX));
  }
}

/**
   Calculate the speed if not close to pole
   go forward or backward coresponding to the location of detection with respect to the frame
   Only apply with the setup of one side camera
*/
int CalcSpeed_demo (float distance, double thetaX) // i think is may be wrong
{
   int mySpeed =0;

//  if (distance <= stopDistance)
//  {
//    mySpeed = stopSpeed;
//    dir_forward = !dir_forward;
//  }  

    mySpeed = stopSpeed + (thetaX * speedRange)/horRange;

    if (mySpeed < stopSpeed - speedRange) //if the speed is below the expected min speed = stop - range
    {
      mySpeed = stopSpeed - speedRange;
    }
    else if (mySpeed > stopSpeed + speedRange) //if speed is above the expected max speed spped = stop + range
    {
      mySpeed = stopSpeed + speedRange;
    }

    if (mySpeed > stopSpeed - speedSafety && mySpeed < stopSpeed) mySpeed = stopSpeed;
    else if (mySpeed < stopSpeed + speedSafety && mySpeed > stopSpeed) mySpeed = stopSpeed;
 
  return mySpeed;
}

/*
  Get the Ultrasnonic reading and perform conversion from analog to centimeter
  Reading from USPin1 if isFront is true meaning the robot is going forward (away from the charging station)
  Reading from USPin2 if isFront is false -> going backward (toward the charging station)
*/
float getUltrasonicDistance(bool isFront) // returns distance in centimeters
{
  int distance = 0;
  if (isFront) distance = analogRead(USPin1);
  else distance = analogRead(USPin2);

  return (distance / 1024.0) * 512 * 2.54;
}

int BitShiftCombine( unsigned char x_high, unsigned char x_low)
{
  int combined;
  combined = x_high;              //send x_high to rightmost 8 bits
  combined = combined << 8;       //shift x_high over to leftmost 8 bits
  combined |= x_low;                 //logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
  return combined;
}
