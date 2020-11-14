#SimpleState.py
from enum import Enum, auto
import time
import sys
from io import StringIO

class States (Enum):
    STOP = auto()
    DETECT = auto()
    GOBACK = auto()

class MyState(object):
"""define a state object which provide some utility functions for the individual state machine"""

    def __init__(self):
        self.__state = States.DETECT
        print "current state: ", str(self)
    def set_state(self,state):
        self.__state = state
    def get_state (Self, state):
        return self.__state

class ControlDetection:
    my_state = MyState()
    stopSpeed = 1500
    stopDistance = 30
    speedRange = 100
    distanceRange = 50
    forward = True
    detectFront = False
    speed = 0
    #detectBack = False

    #Bird Detect Object in here
    def __init__(self):
        my_state.set_state(States.DETECT)

    def calcSpeed(distance, isFront): #1 for front and 0 for left
        #calculate the distance to move
        if (isFront):
            forward = True
            speed = stopSpeed + speedRange - (distanceRange - distance) / distanceRange * speedRange
            if (speed < stopSpeed - speedRange):
                 speed = stopSpeed - speedRange
            elif (speed > stopSpeed + speedRange):
                 speed = stopSpeed + speedRange

            print (distance, " ", speed)
        return speed

    def Run(distance, isFront):
        detect = True

        if my_state.get_state() == States.STOP:
            if distance <= stopDistance and isFront:
                speed = stopSpeed
                my_state.set_state(States.GOBACK)
                if (isFront):
                    forward = false
                else:
                    forward = true
                return speed
            my_state.set_state(States.DETECT)
            if (detect)
                return calcSpeed(distance, isFront)  # if not stop then keep moving

        elif my_state.get_state() == States.GOBACK:
            if (isFront):
                if (forward):
                    my_state.set_state(States.DETECT)
                if (distance < distanceRange and not forward):
                    speed = 1450
                elif (distance > distanceRange - 10):
                    speed = 1600
                    forward = true


        elif my_state.get_state() == States.DETECT:
            if (detectFront):
                forward = true
                my_state.set_state(states.STOP)
                return calcSpeed(distance, isFront)
            #elif (detectBack):
            else:
                return stopSpeed #don't move if don't detect anything



