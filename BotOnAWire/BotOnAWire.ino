#include <ESC.h>
#include <ArduinoJson.h>
#include <SharpIR.h>

#define IRPin A0
int distance_cm;

class: sharpIR mySensor = SharpIR(IRPin, model);

void setup() {
  Serial.begin(9600);
}
void loop() {
  if (Serial.available() > 0) {
    //{"speed": 1500}
    String json = Serial.readStringUntil('\n');
    StaticJsonDocument<200> docIn;
    
    DeserializationError error = deserializeJson(docIn, json);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    int motorSpeed = docIn["speed"];

    //Serial.println("Motor Speed: " + String(motorSpeed));

    StaticJsonDocument<200> doc;
    doc["frontDistance"] = 10;
    doc["backDistance"] = 11;
    serializeJson(doc, Serial);
    Serial.println("");
  }
}

