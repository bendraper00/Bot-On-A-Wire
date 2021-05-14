#include "DirectionalSound.h"
#include <Arduino.h>

void DirectionalSound::init(){
    pinMode(PWMOutPin, OUTPUT);
    pinMode(AudioIn, INPUT);
    TCCR3A = 0xA8;
    TCCR3B = 0x11;
    ICR3 = 400;
    OCR3A = 0; //should be digital pin 5
}
void DirectionalSound::update(){
   OCR3A =  map(analogRead(AudioIn),0,1023,0,400);
}
