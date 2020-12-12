#ifndef DIRECTIONALSOUND_H
#define DIRECTIONALSOUND_H
#include <Arduino.h>

class DirectionalSound{
    private:
        const int PWMOutPin = 11;
        const int AudioIn = A3;
    public:
        void init();
        void update();
};

#endif