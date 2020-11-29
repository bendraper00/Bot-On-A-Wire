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
    try:
        ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=2.5)
    except:
        try:
            ser = serial.Serial('/dev/ttyUSB1', 19200, timeout=2.5)
        except:
            print("not usb0 or usb1")
    ser.flush()
    out = "\n"
    ser.write(out.encode())
    while True:
        #timeOld = time.time()
        #print ("sending 100")
        #out = json.dumps(100) + "\n"
        #ser.write(out.encode())
        
        area, center = Detect()
        #objects = []
        #for i in range(15):
        #    if(area[i] > 0):
        #        objects.append({"area": area[i], "xCord" : center[i][0], "yCord" :  center[i][1]})
        #data= { "detections": objects}
        #out = json.dumps(data) + "\n"
        #ser.write(out.encode())
        
        objects = ""
        for i in range(15):
            if (area[i] > 0):
                if i != 0 and len(objects) != 0:
                    objects = objects + ","
                objects = objects + str(area[i])+ " " + str(center[i][0] )+ " "+str(center[i][1])
        objects = objects + "\n"
        ser.write(objects.encode())
        #print (sys.getsizeof(out))
        print(objects)
        #print(time.time()-timeOld)

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

        #hello = sensorInput["hello"]

        #print(str(hello) )
        #time.sleep(1)
        #out = json.dumps({"speed": calcSpeed(front, 1)}) + "\n"
        #ser.write(out.encode())
        
        