from pyttsx3 import engine
import pyttsx3
engine=pyttsx3.init()
voices=engine.getProperty("voices")
engine.setProperty("voice",voices[70].id)
# print(len(voices))
# print(voices[70].id)
# for voice in voices:
#     print(voice.id)
engine.say("xin chào")
engine.runAndWait()