#include "DirectionalSound.h"

void DirectionalSound::init(){
    pinMode(PWMOutPin, OUTPUT);
    pinMode(AudioIn, INPUT);
    TCCR1A = 0xA8;
    TCCR1B = 0x11;
    ICR1 = 400;
    OCR1C = 0;
}
void DirectionalSound::update(){
   OCR1C =  map(analogRead(AudioIn),0,1023,0,400);
}
