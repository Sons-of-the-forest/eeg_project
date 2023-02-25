import RPi.GPIO as GPIO

from time import sleep

GPIO.setwarnings(False)

GPIO.setmode(GPIO.BOARD)

GPIO.setup(15, GPIO.OUT, initial=GPIO.LOW)

sleep(1)

GPIO.output(15, GPIO.HIGH)

 

while True:

 

 sleep(1)

 print("while")

 