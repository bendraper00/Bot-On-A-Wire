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

SoftwareSerial mySerial(2, 3); // RX, TX

const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;

static volatile uint8_t *buffer;
#define bufferSize 10



uint16_t throttle = 0;
uint16_t target = 0;

typedef struct {
    uint8_t dataAge;
    int8_t temperature;  // C degrees
    int16_t voltage;     // 0.01V
    int32_t current;     // 0.01A
    int32_t consumption; // mAh
    int16_t rpm;         // 0.01erpm
} escSensorData_t;

escSensorData_t escSensorData;

void setup() {
  Serial.begin(115200);
  mySerial.begin(115200);


  // Notice, all pins must be connected to same PORT
  esc1.attach(7);  
  esc1.setThrottle(throttle);
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  delay(5000);
  
}

void loop() {
  /*
  if (Serial.available()>0){
    target = Serial.parseInt();
    if (target>2047)
      target = 2047;
    //esc1.setThrottle(target);
    Serial.print(target, DEC);
    Serial.print("\t");
  }
  
  if (throttle<48){
    throttle = 48;
  }
  if (target<=48){
    esc1.setThrottle(target);
  }else{
    if (target>throttle){
      throttle ++;
      esc1.setThrottle(throttle);
    }else if (target<throttle){
      throttle --;
      esc1.setThrottle(throttle);
    }
  }*/
  
  for (int i = 50; i < 1000; i++) {
    esc1.setThrottle(i);
    //Serial.println(i);
    readAndPrintSerial();
    delay(10);
  }
  for (int i = 1000; i > 50; i--) {
    esc1.setThrottle(i);
    //Serial.println(i);
    readAndPrintSerial();
    delay(10);
  }

  for (int i = 1100; i < 2000; i++) {
    esc1.setThrottle(i);
    //Serial.println(i);
    readAndPrintSerial();
    delay(10);
  }
  for (int i = 2000; i > 1100; i--) {
    esc1.setThrottle(i);
    //Serial.println(i);
    readAndPrintSerial();
    delay(10);
  }
  delay(1000);

  
}

void readAndPrintSerial() {
  if (mySerial.available()) {
    //String myInput = mySerial.readBytes(buffer, bufferSize);
    uint8_t inBuffer[bufferSize+1];
    byte bufIndx = 0;
    
    if (mySerial.available() > 0) {
       while (bufIndx < bufferSize) {
          inBuffer[bufIndx] = mySerial.read();
          bufIndx ++;
       }
       inBuffer[bufIndx] = '\0';
    }
    float temp = inBuffer[0];
    float voltage = BitShiftCombine(inBuffer[1], inBuffer[2])/100.;
    float current = BitShiftCombine(inBuffer[3], inBuffer[4])/100.;
    float consumption = BitShiftCombine(inBuffer[5], inBuffer[6]);
    float rpm = BitShiftCombine(inBuffer[7], inBuffer[8]);
    if (consumption != 0 || rpm < 0 || rpm > 1500) {
      temp = 0;
      voltage = 0;
      current = 0;
      consumption = 0;
      rpm = 0;
      return;
    }
    Serial.print("temp: ");
    Serial.print(temp);
    Serial.print(", voltage: ");
    Serial.print(voltage);
    Serial.print(", current: ");
    Serial.print(current);
    Serial.print(", consumption: ");
    Serial.print(consumption);
    Serial.print(", RPM: ");
    Serial.println(rpm);
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
