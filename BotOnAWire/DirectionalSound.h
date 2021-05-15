#ifndef DIRECTIONALSOUND_H
#define DIRECTIONALSOUND_H
#include <Arduino.h>

class DirectionalSound{
    private:
        const int PWMOutPin = 5;
        const int frequency = 1000;
        
    public:
        void init();
        void update();
        void on();
        void off();
};

#endif
