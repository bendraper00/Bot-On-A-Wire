//#include <ESC.h>
#include "DShot.h"
#include <SoftwareSerial.h>
#include "Lights.h"

#include "DirectionalSound.h"
#include "MedianFilter.h"

#define USPin1 A0  //front
#define USPin2 A2    //back
#define arrayLength 10

//ESC myESC1 (8, 1000, 2000, 2000);
//ESC myESC2 (9, 1000, 2000, 2000);

DShot esc1;
DShot esc2;

SoftwareSerial mySerial2(4, 5); // RX, TX
SoftwareSerial mySerial1(2, 3); // RX, TX

const byte telemetryPin1 = 2;
const byte telemetryPin2 = 2;
volatile byte st = LOW;

static volatile uint8_t *buffer;
#define bufferSize 10

uint16_t throttle = 0;
uint16_t target = 0;

void readAndPrintSerial();

typedef struct {
  uint8_t dataAge;
  int8_t temperature;  // C degrees
  int16_t voltage;     // 0.01V
  int32_t current;     // 0.01A
  int32_t consumption; // mAh
  int16_t rpm;         // 0.01erpm
} escSensorData_t;

escSensorData_t escSensorData;


DirectionalSound dirSound;
Lights strobe;



//bool forward = true;
//int motorSpeed = 1500;
//bool dir_forward = true;
//int stopSpeed = 1500;
//double stopDistance = 30;
//int speedRange = 130;  //+- from 1500
//int patrollingSpeed = 80;
//int speedSafety = 50;
//int horRange = 640;
//double distanceRange = 50;
//float frontDist =0;
//float backDist =0;
bool webcam = true;
unsigned long lastDetect = 0;
float forwardDistances[arrayLength];
bool forward = true;
bool dir_forward = true;

int minSpeed = 49; 
int maxSpeed = 2048;
int midSpeed = 1048; //stop

int stopSpeed = 1048;
int speedRange = 200; //1000 for max
int speedSafety = 50; //Figure this out
int patrollingSpeed = 105; //Figure this out
int motorSpeed = midSpeed + patrollingSpeed;
double voltage = -1;
double stopDistance = 35;
int horRange = 640;
double distanceRange = 50;
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
  mySerial1.begin(115200);
  mySerial2.begin(115200);

  Serial.setTimeout(10000);
  Serial.println("Hello");
  Serial.end();

  //dirSound.init();
  //  myESC1.arm();
  //  myESC2.arm();
  //  myESC1.speed(1500);
  //  myESC2.speed(1500);

  // Notice, all pins must be connected to same PORT
  esc1.attach(6);
  esc2.attach(7);
  pinMode(telemetryPin1, INPUT_PULLUP);
  pinMode(telemetryPin2, INPUT_PULLUP);
  setMotorSpeeds(throttle);
  delay(5000);

  pinMode(13, OUTPUT);
  Serial.begin(19200);
  strobe.init();
}


void loop() {
  frontDist.push(getUltrasonicDistance(true) +6 ); //front == true// front Dist tends to read high
  backDist.push( getUltrasonicDistance(false) + 8); //back dist tends to read low

 /*   Serial.print(frontDist.read());
   Serial.print(" ");
    Serial.println(backDist.read());*/
  //addToArray(frontDist);
  if(voltage > 0 && voltage < 9.7){
    state = GOHOME;
  }

  if (Serial.available() > 0) {
    //Serial.println("detect\n");
    
    Serial.print("go ");
    Serial.println(webcam);
    
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
        forward = false;
      } else if ( backDist.read() <= stopDistance) {
        //Serial.print("FORWARD");
        forward = true;
      } if (forward) {
        motorSpeed = minSpeed + patrollingSpeed;
      } else if (!forward) {
        motorSpeed = midSpeed + patrollingSpeed;
      }
    }
    lastInput = millis();
  }else if (state == GOHOME){
    strobe.off();
    if(!backDist.read() >= stopDistance){
      motorSpeed = midSpeed + patrollingSpeed;
      forward = false;
      lastInput = millis();
    }else{
      if(lastInput- millis() < 2000){ // just continue moving towards the docking station for 2 secs untill we have docking station detection
        motorSpeed = midSpeed + patrollingSpeed*0.75;
      }else{
        motorSpeed = stopSpeed;
        forward = true;
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
  //  myESC1.speed(motorSpeed);
  //  myESC2.speed(motorSpeed);

  strobe.flash();
  setMotorSpeeds(motorSpeed);
  //  delay(2000);
  //  setMotorSpeeds(midSpeed);
  //  delay(2000);
  //  setMotorSpeeds(midSpeed + speedRange);
  //  Serial.println(midSpeed + speedRange);
  //  delay(5000);



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
    forward = true;
    return (CalcSpeed_demo (frontDist.read(), thetaX));
  } else {
    forward = false;
    return (CalcSpeed_demo (backDist.read(), thetaX));
  }
}

/**
   Calculate the speed if not close to pole
   go forward or backward coresponding to the location of detection with respect to the frame
   Only apply with the setup of one side camera
*/
int CalcSpeed_demo (float distance, double thetaX)
{
  int mySpeed = 0;
  int baseSpeed = 0;
  if (distance <= stopDistance ) {
    mySpeed = stopSpeed;
    forward = !forward;
  }

  else {
    
    if (thetaX < 0) //go backward toward docking station -> speed = midspeed + change in speed
    {
      mySpeed =  -(thetaX * speedRange) / horRange; //calculates the speed proprtional to the position of the detection (1048 + range)
      baseSpeed = midSpeed;
    }
    else if (thetaX > 0)//go forward away from docking station -> speed = minspeed + change in speed (49 -> 49 + range)
    {
      mySpeed =  ((thetaX * speedRange)) / horRange; //calculates the speed proprtional to the position of the detection
      baseSpeed = minSpeed;
    }

   if (mySpeed <  speedSafety){
      mySpeed = 0;  //prevent going too low can harm motor
      baseSpeed = stopSpeed;
    }else if(mySpeed > 150){
      mySpeed = 150;
    }
    
  }
  return mySpeed + baseSpeed;
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

//void addToArray(float dist)
//{
//  for (int k = arrayLength - 1; k > 0; k--)
//  {
//    float old = forwardDistances[k - 1];
//    forwardDistances[k] = old;
//    //Serial.print(old);
//  }
//  forwardDistances[0] = dist;
//  //Serial.println(forwardDistances[0]);
//}
//
//bool tooClose(float dist, int valid)
//{
//  int count = 0;
//  for (int k = 0; k < arrayLength - 1; k++)
//  {
//    if (forwardDistances[k] < dist) count++;
//  }
//  //Serial.println(count);
//  return count >= valid;
//}
//
//float frontAvg()
//{
//  int count = 0;
//  for (int k = 0; k < arrayLength - 1; k++)
//  {
//    count += forwardDistances[k];
//  }
//  float avg = count / arrayLength;
//  //Serial.println(avg);
//  return avg;
//}


/* set motor speed for DShot */
void setMotorSpeeds(int motorSpeed) {
  esc1.setThrottle(motorSpeed);
  esc2.setThrottle(motorSpeed);
  readAndPrintSerial();  //TODO: This is the telemetry part. WIP
}

int BitShiftCombine( unsigned char x_high, unsigned char x_low)
{
  int combined;
  combined = x_high;              //send x_high to rightmost 8 bits
  combined = combined << 8;       //shift x_high over to leftmost 8 bits
  combined |= x_low;                 //logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
  return combined;
}

void readAndPrintSerial() {
  if (mySerial1.available()) {
    //String myInput = mySerial.readBytes(buffer, bufferSize);
    uint8_t inBuffer1[bufferSize + 1];
    byte bufIndx1 = 0;

    if (mySerial1.available() > 0) {
      while (bufIndx1 < bufferSize) {
        inBuffer1[bufIndx1] = mySerial1.read();
        bufIndx1 ++;
      }
      inBuffer1[bufIndx1] = '\0';
      mySerial2.listen();
    }
    float temp1 = inBuffer1[0];
    float voltage1 = BitShiftCombine(inBuffer1[1], inBuffer1[2]) / 100.;
    float current1 = BitShiftCombine(inBuffer1[3], inBuffer1[4]) / 100.;
    float consumption1 = BitShiftCombine(inBuffer1[5], inBuffer1[6]);
    float rpm1 = BitShiftCombine(inBuffer1[7], inBuffer1[8]) * 100 / 6;
    if (consumption1 == 0 && rpm1 > 0 && rpm1 < 1500) {
      //TODO test with no vision
      /*Serial.print("Motor 1: ");
      Serial.print("temp: ");
      Serial.print(temp1);
      Serial.print(", voltage: ");
      Serial.print(voltage1);*/
      voltage = voltage1;
      /*Serial.print(", current: ");
      Serial.print(current1);
      Serial.print(", consumption: ");
      Serial.print(consumption1);
      Serial.print(", RPM: ");
      Serial.println(rpm1);*/
    }

  }

  mySerial2.listen();
  if (mySerial2.available()) {
    //String myInput = mySerial.readBytes(buffer, bufferSize);
    uint8_t inBuffer2[bufferSize + 1];
    byte bufIndx2 = 0;

    if (mySerial2.available() > 0) {
      while (bufIndx2 < bufferSize) {
        inBuffer2[bufIndx2] = mySerial2.read();
        bufIndx2 ++;
      }
      inBuffer2[bufIndx2] = '\0';
      mySerial1.listen();
    }
    float temp2 = inBuffer2[0];
    float voltage2 = BitShiftCombine(inBuffer2[1], inBuffer2[2]) / 100.;
    float current2 = BitShiftCombine(inBuffer2[3], inBuffer2[4]) / 100.;
    float consumption2 = BitShiftCombine(inBuffer2[5], inBuffer2[6]);
    float rpm2 = BitShiftCombine(inBuffer2[7], inBuffer2[8]) * 10;
    if (consumption2 == 0 && rpm2 > 0 && rpm2 < 1500) {
      //TODO test with no vision

      /*Serial.print("Motor 2: ");
      Serial.print("temp: ");
      Serial.print(temp2);
      Serial.print(", voltage: ");
      Serial.print(voltage2);*/
      voltage = (voltage + voltage2)/2;
      /*Serial.print(", current: ");
      Serial.print(current2);
      Serial.print(", consumption: ");
      Serial.print(consumption2);
      Serial.print(", RPM: ");
      Serial.println(rpm2);*/
    }
  }
}
