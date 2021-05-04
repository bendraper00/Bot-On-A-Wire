#include "Lights.h"

int lastOff = 0;

void Lights::init(){
    pinMode(lightsPin, OUTPUT);
}
void Lights::on(){
    active =true;
}
void Lights::off(){
    active = false;
}
void Lights::update(){
    
    if(active && millis() s- lastOff < 1000/frequency)){
        digitalWrite(lightsPin, HIGH);
    }else{
        digitalWrite(lightsPin, LOW);
        lastOff = millis();
    }
}