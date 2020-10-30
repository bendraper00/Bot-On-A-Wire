# This program will let you test your ESC and brushless motor.
# Make sure your battery is not connected if you are going to calibrate it at first.
# Since you are testing your motor, I hope you don't have your propeller attached to it otherwise you are in trouble my friend...?
# This program is made by AGT @instructable.com. DO NOT REPUBLISH THIS PROGRAM... actually the program itself is harmful                                             pssst Its not, its safe.

import os     #importing os library so as to communicate with the system
import time   #importing time library to make Rpi wait because its too impatient 
os.system ("sudo pigpiod") #Launching GPIO library
time.sleep(1) # As i said it is too impatient and so if this delay is removed you will get an error
import pigpio #importing GPIO library
from Bluetin_Echo import Echo

ESC= 4  #Connect the ESC in this GPIO pin
triggerPin = 27 
echoPin = 17

pi = pigpio.pi();
pi.set_servo_pulsewidth(ESC, 0)
echo = Echo(triggerPin, echoPin, 340)

max_value = 2000 #change this if your ESC's max value is different or leave it be
min_value = 700  #change this if your ESC's min value is different or leave it be
#print ("For first time launch, select calibrate")
#print ("Type the exact word for the function you want")
#print ("calibrate OR manual OR control OR arm OR stop")

def manual_drive(): #You will use this function to program your ESC if required
    print ("You have selected manual option so give a value between 0 and you max value")    
    while True:
        inp = input()
        if inp == "stop":
            stop()
            break
        elif inp == "control":
            control()
            break
        elif inp == "arm":
            arm()
            break   
        else:
            pi.set_servo_pulsewidth(ESC,inp)
                
def calibrate():   #This is the auto calibration procedure of a normal ESC
    pi.set_servo_pulsewidth(ESC, 0)
    print("Disconnect the battery and press Enter")
    inp = input()
    if inp == '':
        pi.set_servo_pulsewidth(ESC, max_value)
        print("Connect the battery NOW.. you will here two beeps, then wait for a gradual falling tone then press Enter")
        inp = input()
        if inp == '':            
            pi.set_servo_pulsewidth(ESC, min_value)
            print ("Wierd eh! Special tone")
            time.sleep(7)
            print ("Wait for it ....")
            time.sleep (5)
            print ("Im working on it, DONT WORRY JUST WAIT.....")
            pi.set_servo_pulsewidth(ESC, 0)
            time.sleep(2)
            print ("Arming ESC now...")
            pi.set_servo_pulsewidth(ESC, min_value)
            time.sleep(1)
            print ("See.... uhhhhh")
            control() # You can change this to any other function you want
            
def control(): 
    print ("I'm Starting the motor, I hope its calibrated and armed, if not restart by giving 'x'")
    time.sleep(1)
    speed = 1500    # change your speed if you want to.... it should be between 700 - 2000
    print ("Controls - a to decrease speed & d to increase speed OR q to decrease a lot of speed & e to increase a lot of speed")
    while True:
        pi.set_servo_pulsewidth(ESC, speed)
        inp = input()
        #dist = echo.read('cm', 5)
        #print ("Distance = %.1f cm" % dist)
        
        if inp == "q":
            speed -= 100    # decrementing the speed like hell
            print ("speed = %d" % speed)           
        elif inp == "e":    
            speed += 100    # incrementing the speed like hell
            print ("speed = %d" % speed)
        elif inp == "d":
            speed += 10     # incrementing the speed 
            print ("speed = %d" % speed)
        elif inp == "a":
            speed -= 10     # decrementing the speed
            print ("speed = %d" % speed)
        elif inp == "stop":
            #stop()          #going for the stop function
            speed = 1500
            break
        elif inp == "manual":
            manual_drive()
            break
        elif inp == "arm":
            arm()
            break   
        else:
            print ("WHAT DID I SAID!! Press a,q,d or e")
            
def arm(): #This is the arming procedure of an ESC 
    print ("Connect the battery and press Enter")
    inp = input()    
    if inp == '':
        pi.set_servo_pulsewidth(ESC, 0)
        time.sleep(1)
        pi.set_servo_pulsewidth(ESC, max_value)
        time.sleep(1)
        pi.set_servo_pulsewidth(ESC, min_value)
        time.sleep(1)
        #control()
        
        
def stop(): #This will stop every action your Pi is performing for ESC ofcourse.
    pi.set_servo_pulsewidth(ESC, 0)
    pi.stop()
    
def ultra(beta, dist): 
    #dist = low_pass_filter (echo.read('cm',2), dist, float(beta))
    #dist = round (dist, 2)
    dist = echo.read('cm', 2)
    if (dist <= 0):
        dist = 250
    elif (dist >=300):
        dist = 250
        
    if (len(array) >= 3):
        array.pop(2)
        array.insert(0, dist)
        
        if (len(array) >=3):
            counter = 0
            print (array)
            for a in array:
                if (a < 50 and a >0):
                    counter = counter + 1
                print (a, " : ", counter)
            if (counter >=2):
                pi.set_servo_pulsewidth(ESC, 1500)
                print ("hi")
                print(time.ctime())
                time.sleep (5)
                print (time.ctime())
    pi.set_servo_pulsewidth(ESC, math(dist))
    
def math(distance):
    speed = 1500 - (distance - 50)/220 *(100)
    if (speed < 1400):
        speed = 1400
    elif (speed >1500) :
        speed = 1500
    print (distance, " ", speed)
    return speed

def low_pass_filter(value, previousValue, beta):  #low pass filter
    if (previousValue > 300):
        previousValue = 250
    smoothValue = previousValue - (beta *(previousValue - value))
    return smoothValue
    
#This is the start of the program actually, to start the function it needs to be initialized before calling... stupid python.    

if __name__== '__main__':
    try:
#         arm() #arn before and only one
        pi.set_servo_pulsewidth(ESC, 1500)
        time.sleep(1)
        speed = 1500
        dist =0
        global array
        array = []
        while True:
            beta = 0.5 # 0<x<1
            dist = 0   
            ultra (beta, dist)
            #pi.set_servo_pulsewidth(ESC, speed)
            #if input == "s":
            #    speed = 1500
            #control()
          
    except KeyboardInterrupt:
        pi.stop()
        GPIO.cleanup()
