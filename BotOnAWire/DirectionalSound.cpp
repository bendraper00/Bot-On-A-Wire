#include "DirectionalSound.h"
#include <Arduino.h>
uint8_t  sine_wave[16] = {
4,5,6,7,7,7,6,5,
4,2,1,0,0,0,1,2
};
void DirectionalSound::init(){
    pinMode(PWMOutPin, OUTPUT);
    TCCR3A = 0xA8;
    TCCR3B = 0x11;
    ICR3 = 400;
    OCR3A = 0; //should be digital pin 5
}
void DirectionalSound::update(){
  int index = ((millis()*frequency*16)/1000)%16;
   OCR3A =  map(sine_wave[index],0,7,0,400); // map(analogRead(AudioIn),0,1023,0,400);
}
