#ifndef GIMBAL_H
#define GIMBAL_H
#include <Arduino.h>
#include "SparkFun_TB6612.h"

#define YENCA 20
#define YENCB 21
#define PENCA 18
#define PENCB 19
#define LIMITPITCH 11
#define LIMITYAW 4
#define AIN1 13
#define BIN1 8
#define AIN2 6
#define BIN2 9
#define PWMA 30
#define PWMB 10
#define STBY 12
#define YCPR 1080 //yaw encoder counts per revolution (of end effector)
#define PCPR 17310 //very back of the envolope pitch encoder counts per revolution (of end effector)
    class Gimbal{
        private:

            void home();
            void PIyaw();
            void PIpitch();

        public:
            void init();
            void update();
            void setYawAngle(double);
            void setPitchAngle(double);
            double getYawAngle();
            double getPitchAngle();

    };
#endif
