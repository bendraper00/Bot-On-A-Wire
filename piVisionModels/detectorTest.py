import detector
dc = detector.detector()


def main():
    for x in range (200):
       print(dc.detect())
    dc.exit()

main()