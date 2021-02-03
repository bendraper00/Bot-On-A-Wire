import tensorflow as tf

"""# Convert the model
converter = tf.lite.TFLiteConverter.from_saved_model("C:/Bot-On-A-Wire/tensorflow1/models/research/object_detection/tflite_model/saved_model") # path to the SavedModel directory
tflite_model = converter.convert()

# Save the model.
with open('detect.tflite', 'wb') as f:
  f.write(tflite_model)"""
'''## TFLite Conversion
# Before conversion, fix the model input size
model = tf.saved_model.load("C:/Bot-On-A-Wire/tensorflow1/models/research/object_detection/ssd_mobilenet_v1_fpn_640x640_coco17_tpu-8/saved_model")
model.signatures[tf.saved_model.DEFAULT_SERVING_SIGNATURE_DEF_KEY].inputs[0].set_shape([1, 640, 640, 3])
tf.saved_model.save(model, "saved_model_updated", signatures=model.signatures[tf.saved_model.DEFAULT_SERVING_SIGNATURE_DEF_KEY])
# Convert
converter = tf.lite.TFLiteConverter.from_saved_model(saved_model_dir='saved_model_updated', signature_keys=['serving_default'])
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
tflite_model = converter.convert()

## TFLite Interpreter to check input shape
interpreter = tf.lite.Interpreter(model_content=tflite_model)
interpreter.allocate_tensors()

# Get input and output tensors.
input_details = interpreter.get_input_details()
output_details = interpreter.get_output_details()

# Test the model on random input data.
input_shape = input_details[0]['shape']
print(input_shape)'''
model = tf.saved_model.load("")
concrete_func = model.signatures[tf.saved_model.DEFAULT_SERVING_SIGNATURE_DEF_KEY]
concrete_func.inputs[0].set_shape([1, 640, 640, 3])
converter = tf.lite.TFLiteConverter.from_concrete_functions([concrete_func])

converter = tf.lite.TFLiteConverter.from_saved_model("",signature_keys=['serving_default'])
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.experimental_new_converter = True
converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS, tf.lite.OpsSet.SELECT_TF_OPS]
tflite_model = converter.convert()

# Save the model.
with open('detect.tflite', 'wb') as f:
  f.write(tflite_model)
print("done")
