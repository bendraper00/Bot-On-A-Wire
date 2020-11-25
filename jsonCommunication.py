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
    box, confidence = detect.detect()
    return detect.filterData(box, confidence)
    
if __name__ == '__main__':
    area, center = Detect()
    try:
        ser = serial.Serial('/dev/ttyUSB0', 9600, timeout=1)
    except:
        try:
            ser = serial.Serial('/dev/ttyUSB1', 9600, timeout=1)
        except:
            print("not usb0 or usb1")
    ser.flush()
    out = "\n"
    ser.write(out.encode())
    while True:
       
        objects = []
        for i in range(15):
            if(area[i] > 0):
                objects.append({"area": area[i], "xCord" : center[i][0], "yCord" :  center[i][1]})
        data= { "detections": objects}
        out = json.dumps(data) + "\n"
        ser.write(out.encode())
        print(out)
        try:
            line = ser.readline().decode('utf-8')
        except:
            print("An exception occurred")
        line = line.rstrip().replace('\'', '\"').replace('\n','')
        line = eval(line)
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
        
        