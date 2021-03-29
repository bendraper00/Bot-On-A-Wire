import detector
dc = detector.detector()


def main():
    webcam = True
    while True:
        print(dc.filterData(dc.detect(webcam)))
    dc.exit()

main()
