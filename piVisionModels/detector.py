import jetson.utils


#camera = jetson.utils.videoSource("csi://0")#/home/jetson/test.mp4", argv=["--input-codec=mpeg4"])
camera1 = jetson.utils.videoSource("/dev/video0")
#display = jetson.utils.videoOutput("liveTest.mp4") 

import jetson.inference
net = jetson.inference.detectNet(argv=["--model=/home/jetson/jetson-inference/python/training/detection/ssd/models/cormorantModel/ssd-mobilenet.onnx","--labels=/home/jetson/jetson-inference/python/training/detection/ssd/models/cormorantModel/labels.txt", "--input-blob=input_0", "--output-cvg=scores","--output_bbox=boxes"] , threshold=0.5)


imH = camera1.GetHeight()
imW= camera1.GetWidth()
min_conf_threshold = 0.6

class detector:
	def detect(self,webcam):
		frame = None
		if webcam:
			frame = camera1.Capture()
		#else:
			#frame = camera.Capture()
		captures = []
		if frame:
			captures = net.Detect(frame)
			#display.Render(frame)
		return captures 
    
	def filterData(self, detections):
		areas = []
		centers = []
		distFromEdge = 5
		fractOfFrame = 0.75
		for i in detections:
			ymin = i.Top
			xmin = i.Left
			ymax = i.Bottom
			xmax = i.Right
			if ((i.Confidence > min_conf_threshold) and (ymin > distFromEdge) and (xmin > distFromEdge) and (imH - ymax > distFromEdge) and (imW - xmax > distFromEdge)):
				if (imW*imH*fractOfFrame > i.Area):
					areas.append(i.Area)
				else:
					areas.append(-1)  
			else:
				areas.append(-1)
			centers.append(i.Center)
		return areas, centers

