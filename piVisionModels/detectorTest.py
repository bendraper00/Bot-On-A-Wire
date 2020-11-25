import detector
import cv2
dc = detector.detector()


def main():
    while dc.isRunning():
        boxes, conf = dc.detect()
        print(dc.filterData(boxes, conf))
    dc.exit()

main()