import RPi.GPIO as GPIO
import time
from Bluetin_Echo import Echo

GPIO.setmode(GPIO.BCM)  #GPIO mode (BOARD /BCM)

#pin
servoPin = 4 #can change pin later
triggerPin = 27  #can change pin later
echoPin = 17  #can change pin later

#setUp
GPIO.setup(servoPin, GPIO.OUT)
GPIO.setup(triggerPin, GPIO.OUT)
GPIO.setup(echoPin, GPIO.IN)

p = GPIO.PWM(servoPin, 50) #GPIO servoPin for PWN with 50Hz
p.start(0) #Initialization
echo = Echo(triggerPin, echoPin, 340)


if __name__== '__main__':
    try:
        while True:
            #dist = echo.read('cm', 5)
            #print ("Distance = %.1f cm" % dist)
            #time.sleep (0.5)
            p.ChangeDutyCycle(10)
              
            
    #reset by pressing CTRL + C
    except KeyboardInterrupt:
        p.stop()
        GPIO.cleanup()