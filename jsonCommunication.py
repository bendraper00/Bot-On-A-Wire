#!/usr/bin/env python3
import serial
import time
import json
import sys
from io import StringIO
from piVisionModels.detector import*
import threading

detect = detector()
fileName = "detectRecords.txt"
totalDetections = 0
countFrames = 0

def writeRecords():
    myFile = open(fileName, 'a')
    myFile.write(totalDetections/countFrames)
    print(totalDetections/countFrames)
    totalDetections = 0
    countFrames = 0
    myFile.close()

def serialThread(ser, detectOn):
    while True:
        try:
            line = ser.readline().decode('utf-8')
        except:
            line = "hi"
        print(line[0:4])
        if(line[0:4] == 'stop'):
            detectOn = False
        if(line[0:2] == "go"):
            detectOn = True
        time.sleep(1)
    

def Detect(webcam):
    return detect.filterData(detect.detect(webcam))
    
if __name__ == '__main__':
    try:
         ser = serial.Serial('/dev/ttyUSB0', 19200, timeout=2.5)
    except:
        try:
            ser = serial.Serial('/dev/ttyUSB1', 19200, timeout=2.5)
        except:
            print("not AMC0 or usb1")
    ser.flush()
    out = "\n"
    ser.write(out.encode())
    done = False
    detectOn = True
    '''x = threading.Thread(target=serialThread, args=(ser,detectOn,), daemon=True)
    x.start()''' #an attempt at multithreading with serial
    while True:
        #timeOld = time.time()
        #print ("sending 100")
        #out = json.dumps(100) + "\n"
        #ser.write(out.encode())
        timeOld = time.time()
        try:
            line = ser.readline().decode('utf-8') uncomment when ready to have the arduino stop detections
        except:
            line = ""
        print(line)
        webcam = True
        if(line[0:4] == 'stop'):
            detectOn = False
        if(line[0:2] == "go"):
            detectOn = True
            webcam = line[3:4] == "1"
        if detectOn:
            area, center = Detect(webcam)
            print(1/(time.time()-timeOld))
        else:
           area = [0 ,0, 0 , 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 , 0, 0 ,0 ,0]
           center = [0 ,0, 0 , 0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 , 0, 0 ,0 ,0]
        #objects = []
        #for i in range(15):
        #    if(area[i] > 0):
        #        objects.append({"area": area[i], "xCord" : center[i][0], "yCord" :  center[i][1]})
        #data= { "detections": objects}
        #out = json.dumps(data) + "\n"
        #ser.write(out.encode())
        counter = 0
        objects = ""
        for i in range(len(area)):
            if (area[i] > 0):
                if i != 0 and len(objects) != 0:
                    objects = objects + ","
                    print("adding a comma")
                objects = objects + str(area[i])+ " " + str(center[i][0] )+ " "+str(center[i][1])
                counter+= 1
        if (counter ==0):
            objects = "0 0 0"
        objects = objects + "\n"
        totalDetections += counter
        countFrames += 1
        if countFrames >= 24000:
            writeRecords()
        #if (done):
        #print("start")
        print(objects)
        ser.write(objects.encode())
            #print (sys.getsizeof(out))
        
            #print(time.time()-timeOld)

        #try:
            #line = ser.readline().decode('utf-8')
        #except:
            #print("An exception occurred")
        #line = line.rstrip().replace('\'', '\"').replace('\n','')
        #print (line)
        #line = eval(line)
        #if (line == ''):
            #print("empty")
            #continue
        #elif ("Done" in line):
            #done = True
        #hello = sensorInput["hello"]

        #print(str(hello) )
        #time.sleep(1)
        #out = json.dumps({"speed": calcSpeed(front, 1)}) + "\n"
        #ser.write(out.encode())
        
        
