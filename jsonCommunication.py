#!/usr/bin/env python3
import serial
import time
import json
import sys
from io import StringIO
from piVisionModels.detector import*

detect = detector()

#control = ControlDetection()

def Detect():
    #print ("Enter Detect")
    box,label,confidence,count = detect.detect()
    for confi in confidence:
      #  print ("confidence level: %f", confi)
        if confi >= 0.6:
       #     print ("DETECT")
            return True
    return False
    
if __name__ == '__main__':

    list = [1,2,3,4]
    isDetect = Detect()
    try:
        ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
    except:
        try:
            ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
        except:
            print("not usb0 or usb1")
    ser.flush()
    ser.write(out.encode())
    while True:
        #print ("inside while loop")
        out = json.dumps(list) + "\n"
        ser.write(out.encode())
        try:
            line = ser.readline().decode('utf-8')
        except:
            print("An exception occurred")
        line = line.rstrip().replace('\'', '\"').replace('\n','')
        #line = eval(line)
        if (line == ''):
            print("empty")
            continue
        
        try:
            sensorInput = json.loads(line)
        except:
            print("invalid line: " + line)

        hello = sensorInput["hello"]

        print(str(hello) )
        #time.sleep(1)
        #out = json.dumps({"speed": calcSpeed(front, 1)}) + "\n"
        #ser.write(out.encode())
        
        