import simpleaudio as sa

wave_obj = sa.WaveObject.from_wave_file('C:/Users/Nick Grunski/Desktop/test.wav')
play_obj = wave_obj.play()
while(True):
    print('hi')
