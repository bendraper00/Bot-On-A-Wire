#ifndef GIMBAL_H
#define GIMBAL_H
#include <Arduino.h>
#include <SparkFun_TB6612.h>

#define YENCA 2
#define YENCB 3
#define PENCA 0
#define PENCB 1
#define LIMITPITCH 11
#define LIMITYAW 4
#define AIN1 13
#define BIN1 8
#define AIN2 6
#define BIN2 9
#define PWMA 5
#define PWMB 10
#define STBY 12
#define YCPR 1080 //yaw encoder counts per revolution (of end effector)
#define PCPR 17310 //very back of the envolope pitch encoder counts per revolution (of end effector)
    class Gimble{
        private:
            const char X = 5;
            long yawCount = -1;
            long pitchCount = -1;
            const char encoderArray[4][4] = {
                {0,-1,1,X},
                {1,0,X,-1},
                {-1,X,0,1},
                {X,1,-1,0}
            };
            int oldValueP =0;
            int oldValueY =0;
            const int offsetA = 1;
            const int offsetB = 1;
            Motor yawMotor = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
            Motor pitchMotor = Motor(BIN1, BIN2, PWMB, offsetB, STBY);
            double Yangle = 0;
            double Pangle = 0;
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

    }
#endif