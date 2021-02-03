#SimpleState.py
from enum import Enum, auto
import time
import sys
from io import StringIO
from piVisionModels.detector import*

class States (Enum):
    STOP = auto()
    DETECT = auto()
    GOBACK = auto()

class MyState(object):
#define a state object which provide some utility functions for the individual state machine

    def __init__(self):
        self.__state = States.DETECT
        print ("current state: " + str(self.__state))
    def set_state(self, state):
        self.__state = state
    def get_state (self):
        return self.__state

stopSpeed = 1500
stopDistance = 30
speedRange = 100
distanceRange = 50
forward = True
detectFront = False
speed = 0
detectBack = False
class ControlDetection:
    global my_state
    my_state = MyState()
    global detect
    detect = detector()
    
    #Bird Detect Object in here
    def __init__(self):
        my_state = MyState()  
        my_state.set_state(States.DETECT)
        

    def calcSpeed(self, distance, isFront): #1 for front and 0 for left
        #calculate the distance to move
        if (isFront):
            #forward = True
            speed = stopSpeed + speedRange - (distanceRange - distance) / distanceRange * speedRange
            if (speed < stopSpeed - speedRange):
                 speed = stopSpeed - speedRange
            elif (speed > stopSpeed + speedRange):
                 speed = stopSpeed + speedRange
            print (distance, " ", speed)
        return speed

    def Detect(self):
        print ("Enter Detect")
        box,label,confidence,count = detect.detect()
        for confi in confidence:
            print ("confidence level: %f", confi)
            if confi >= 0.6:
                print ("DETECT")
                return True
        return False
    
    def Run(self, distance, isFront):
        global my_state, forward
        
        #detectFront = self.Detect()  #this is where the detection method will come in
        #detectBack = True

        if my_state.get_state() == States.STOP:
            if distance <= stopDistance and isFront:
                speed = stopSpeed
                my_state.set_state(States.GOBACK)
                if (isFront): #if forward is too close then go backward
                    forward = False
                else: #if backward is too cloase then go forward
                    forward = True
                return speed
            my_state.set_state(States.DETECT)
            #if (detectFront): #might not need this
                #return self.calcSpeed(distance, isFront)  # if not stop then keep moving

        elif my_state.get_state() == States.GOBACK:
            if (isFront):

                if (distance < distanceRange and not forward):
                    speed = 1450
                elif (distance > distanceRange - 10):
                    speed = 1600
                    forward = True
                    my_state.set_state(States.DETECT)


        elif my_state.get_state() == States.DETECT:
            detectFront = self.Detect()
            if (detectFront):
                forward = True
                my_state.set_state(States.STOP)
                return self.calcSpeed(distance, isFront)
            #elif (detectBack):
            else:
                return stopSpeed #don't move if don't detect anything
        print("CurrentState: " + str(my_state.get_state()))



