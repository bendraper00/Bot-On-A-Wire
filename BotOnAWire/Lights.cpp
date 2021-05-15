#include "Lights.h"


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
    
    if(active){
        digitalWrite(lightsPin, (millis()*frequency/500)%2 == 0);
    }else{
        digitalWrite(lightsPin, LOW);
    }
}
