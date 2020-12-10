#include <DShot.h>
#include <SoftwareSerial.h>



/*
redefine DSHOT_PORT if you want to change the default PORT
Defaults
UNO: PORTD, available pins 0-7 (D0-D7)
Leonardo: PORTB, available pins 4-7 (D8-D11)
e.g.
#define DSHOT_PORT PORTD
*/
DShot esc1;
DShot esc2;

int motorMin = 49;
int motorMid = 1048;
int motorMax = 2048;

SoftwareSerial mySerial2(4, 5); // RX, TX
SoftwareSerial mySerial1(2, 3); // RX, TX

const byte telemetryPin1 = 2;
const byte telemetryPin2 = 2;
volatile byte state = LOW;

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

void setMotorSpeeds(int motorSpeed) {
  esc1.setThrottle(motorSpeed);
  esc2.setThrottle(motorSpeed);
  //Serial.println(motorSpeed);
  readAndPrintSerial();
}

void setup() {
  Serial.begin(115200);
  mySerial1.begin(115200);
  mySerial2.begin(115200);


  // Notice, all pins must be connected to same PORT
  esc1.attach(6);  
  esc2.attach(7);  
  pinMode(telemetryPin1, INPUT_PULLUP);
  pinMode(telemetryPin2, INPUT_PULLUP);
  setMotorSpeeds(throttle);
  delay(5000);
  
}

void loop() {
  /*
  if (Serial.available()>0){
    target = Serial.parseInt();
    setMotorSpeeds(target);
    Serial.print(target, DEC);
    Serial.print("\t");
  }*/
  /*else{
    setMotorSpeeds(throttle);
  }*/

  
  /*for (int i = motorMin; i < motorMid; i++) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(10);
  }
  for (int i = motorMid-1; i > motorMin; i--) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(10);
  }
  setMotorSpeeds(0);
  delay(1000);
  for (int i = motorMid; i < motorMax; i++) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(10);
  }
  for (int i = motorMax-1; i > motorMid; i--) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(10);
  }
  */

  int maxChange = 400;
  int myDelay = 50;
  for (int i = motorMin; i < motorMin + maxChange; i++) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(myDelay);
  }
  for (int i = motorMin + maxChange; i > motorMin; i--) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(myDelay);
  }
  setMotorSpeeds(0);
  delay(1000);
  for (int i = motorMid; i < motorMid + maxChange; i++) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(myDelay);
  }
  for (int i = motorMid + maxChange; i > motorMid; i--) {
    setMotorSpeeds(i);
    //Serial.println(i);
    delay(myDelay);
  }
  
  setMotorSpeeds(0);
  delay(1000);

/*
  for (int i = 50; i < 2000; i++) {
    setMotorSpeeds(i);
    Serial.println(i);
    delay(100);
  }
  for (int i = 2000; i > 50; i--) {
    setMotorSpeeds(i);
    Serial.println(i);
    delay(100);
  }*/
}

void readAndPrintSerial() {
  if (mySerial1.available()) {
    //String myInput = mySerial.readBytes(buffer, bufferSize);
    uint8_t inBuffer1[bufferSize+1];
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
    float voltage1 = BitShiftCombine(inBuffer1[1], inBuffer1[2])/100.;
    float current1 = BitShiftCombine(inBuffer1[3], inBuffer1[4])/100.;
    float consumption1 = BitShiftCombine(inBuffer1[5], inBuffer1[6]);
    float rpm1 = BitShiftCombine(inBuffer1[7], inBuffer1[8]) * 100 / 6;
    if (consumption1 == 0 && rpm1 > 0 && rpm1 < 1500) {
      Serial.print("Motor 1: ");
      Serial.print("temp: ");
      Serial.print(temp1);
      Serial.print(", voltage: ");
      Serial.print(voltage1);
      Serial.print(", current: ");
      Serial.print(current1);
      Serial.print(", consumption: ");
      Serial.print(consumption1);
      Serial.print(", RPM: ");
      Serial.println(rpm1);
    }
    
  }

  mySerial2.listen();
  if (mySerial2.available()) {
    //String myInput = mySerial.readBytes(buffer, bufferSize);
    uint8_t inBuffer2[bufferSize+1];
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
    float voltage2 = BitShiftCombine(inBuffer2[1], inBuffer2[2])/100.;
    float current2 = BitShiftCombine(inBuffer2[3], inBuffer2[4])/100.;
    float consumption2 = BitShiftCombine(inBuffer2[5], inBuffer2[6]);
    float rpm2 = BitShiftCombine(inBuffer2[7], inBuffer2[8]) * 10;
    if (consumption2 == 0 && rpm2 > 0 && rpm2 < 1500) {
      Serial.print("Motor 2: ");
      Serial.print("temp: ");
      Serial.print(temp2);
      Serial.print(", voltage: ");
      Serial.print(voltage2);
      Serial.print(", current: ");
      Serial.print(current2);
      Serial.print(", consumption: ");
      Serial.print(consumption2);
      Serial.print(", RPM: ");
      Serial.println(rpm2);
    }
  }
}



int BitShiftCombine( unsigned char x_high, unsigned char x_low)
{
  int combined;
  combined = x_high;              //send x_high to rightmost 8 bits
  combined = combined<<8;         //shift x_high over to leftmost 8 bits
  combined |= x_low;                 //logical OR keeps x_high intact in combined and fills in                                                             //rightmost 8 bits
  return combined;
}
