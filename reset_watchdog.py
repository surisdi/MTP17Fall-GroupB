import RPi.GPIO as GPIO
import time
import subprocess

GPIO.setmode(GPIO.BCM)

GPIO.setup(15, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.setup(18, GPIO.IN, pull_up_down=GPIO.PUD_UP)

while True:
	"""
	input_state = GPIO.input(15)
	if input_state == False:
	"""
	
	GPIO.wait_for_edge(15,GPIO.FALLING)
	if(not GPIO.input(18)):
		print("Launching Single Mode")
		subprocess.call(['sudo','pkill', '-f', 'main'])
		subprocess.Popen(["sudo","./main"])
		time.sleep(1)
	else:
		print("Network Mode")
		time.sleep(1)
	#subprocess.call()
	
