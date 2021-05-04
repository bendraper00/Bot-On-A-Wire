#include "Lights.h"

int lastSwitch = 0;
bool out = true;

void Lights::init(){
    pinMode(lightsPin, OUTPUT);
}
void Lights::on(){
    active =true;
}
void Lights::off(){
    active = false;
}
void Lights::flash(){
    
    if(active){
        if(millis()- lastSwitch > 1000/frequency){
          out = !out;
          lastSwitch = millis();
        }
        digitalWrite(lightsPin, out);
    }else{
        digitalWrite(lightsPin, LOW);
    }
}
