import detector
import cv2
dc = detector.detector()


def main():
    webcam = True
    while dc.isRunning():
        boxes, conf = dc.detect(webcam)
        print(webcam)
        print(dc.filterData(boxes, conf))
        webcam = not webcam
    dc.exit()

main()