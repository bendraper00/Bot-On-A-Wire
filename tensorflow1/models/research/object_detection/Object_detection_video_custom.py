######## Video Object Detection Using Tensorflow-trained Classifier #########
#
# Author: Evan Juras
# Date: 1/16/18
# Description: 
# This program uses a TensorFlow-trained classifier to perform object detection.
# It loads the classifier and uses it to perform object detection on a video.
# It draws boxes, scores, and labels around the objects of interest in each
# frame of the video.

## Some of the code is copied from Google's example at
## https://github.com/tensorflow/models/blob/master/research/object_detection/object_detection_tutorial.ipynb

## and some is copied from Dat Tran's example at
## https://github.com/datitran/object_detector_app/blob/master/object_detection_app.py

## but I changed it to make it more understandable to me.

# Import packages
import os
import cv2
import tensorflow as tf
import matplotlib
import matplotlib.pyplot as plt
import scipy.misc
import numpy as np
import sys
import time

# This is needed since the notebook is stored in the object_detection folder.
sys.path.append("..")

# Import utilites
from object_detection.utils import label_map_util
from object_detection.utils import config_util
from object_detection.utils import visualization_utils as viz_utils
from object_detection.builders import model_builder



# Name of the directory containing the object detection module we're using
MODEL_NAME = 'inference_graph\saved_model'
VIDEO_NAME = 'test2.mp4'
#test
# Grab path to current working directory
CWD_PATH = os.getcwd()

# Path to frozen detection graph .pb file, which contains the model that is used
# for object detection.
#PATH_TO_CKPT = os.path.join(CWD_PATH,MODEL_NAME,'saved_model.pb')
PATH_TO_CKPT = os.path.join(CWD_PATH,'ssd_mobilenet_v2_fpnlite_640x640_coco17_tpu-8','new_model')
PATH_TO_CFG =  os.path.join(CWD_PATH,'training','ssd_mobilenet_v2_fpnlite_640x640_coco17_tpu-8.config')

# Path to label map file
PATH_TO_LABELS = os.path.join(CWD_PATH,'training','labelmap.txt')
# Enable GPU dynamic memory allocation
gpus = tf.config.experimental.list_physical_devices('GPU')
for gpu in gpus:
    tf.config.experimental.set_memory_growth(gpu, True)

# Load pipeline config and build a detection model
configs = config_util.get_configs_from_pipeline_file(PATH_TO_CFG)
model_config = configs['model']
detection_model = model_builder.build(model_config=model_config, is_training=False)

# Restore checkpoint
ckpt = tf.compat.v2.train.Checkpoint(model=detection_model)
ckpt.restore(os.path.join(PATH_TO_CKPT, 'ckpt-35')).expect_partial()

@tf.function
def detect_fn(image):
    """Detect objects in image."""

    image, shapes = detection_model.preprocess(image)
    prediction_dict = detection_model.predict(image, shapes)
    detections = detection_model.postprocess(prediction_dict, shapes)

    return detections, prediction_dict, tf.reshape(shapes, [-1])
# Path to video
PATH_TO_VIDEO = os.path.join(CWD_PATH,VIDEO_NAME)

category_index = label_map_util.create_category_index_from_labelmap(PATH_TO_LABELS, use_display_name=True)
# Number of classes the object detector can identify
NUM_CLASSES = 1

# Open video file
video = cv2.VideoCapture(PATH_TO_VIDEO)
timeOld = time.time()
frame_width = int(video.get(3))
frame_height = int(video.get(4))
#out = cv2.VideoWriter('output.mp4',cv2.VideoWriter_fourcc(*'mp4v'), 30, (frame_width,frame_height))
while(video.isOpened()):

    # Acquire frame and expand frame dimensions to have shape: [1, None, None, 3]
    # i.e. a single-column array, where each item in the column has the pixel RGB value
    ret, frame = video.read()
    if ret == False:
        break
    frame_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    frame_expanded = np.expand_dims(frame_rgb, axis=0)

    input_tensor = tf.convert_to_tensor(np.expand_dims(frame, 0), dtype=tf.float32)
    detections, predictions_dict, shapes = detect_fn(input_tensor)

    label_id_offset = 1
    frame_with_detections = frame.copy()

    viz_utils.visualize_boxes_and_labels_on_image_array(
          frame_with_detections,
          detections['detection_boxes'][0].numpy(),
          (detections['detection_classes'][0].numpy() + label_id_offset).astype(int),
          detections['detection_scores'][0].numpy(),
          category_index,
          use_normalized_coordinates=True,
          max_boxes_to_draw=200,
          min_score_thresh=.35,
          agnostic_mode=False)
    #output to file
    #out.write(frame_with_detections)

    # All the results have been drawn on the frame, so it's time to display it.
    cv2.imshow('Object detector', frame_with_detections)

    print('fps: ', 1/(time.time()-timeOld))
    # Press 'q' to quit
    if cv2.waitKey(1) == ord('q'):
        break
    timeOld = time.time()
# Clean up
video.release()
#out.release()
cv2.destroyAllWindows()
print('done')
