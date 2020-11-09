import tensorflow as tf

# Convert the model
converter = tf.lite.TFLiteConverter.from_saved_model("C:/Bot-On-A-Wire/tensorflow1/models/research/object_detection/tflite_model/saved_model") # path to the SavedModel directory
tflite_model = converter.convert()

# Save the model.
with open('detect.tflite', 'wb') as f:
  f.write(tflite_model)
