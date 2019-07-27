#!/usr/bin/python3
"""
Python Practical 1
Name: Noel Loxton
Student Number: LXTNOE001
Prac: 1
Date: 4/8/2019
"""

# import Relevant Librares
import RPi.GPIO as GPIO  #import Raspberry Pi GPIO python module
from time import sleep  #import the sleep function from the time module

#event detection method
def button_callback(channel):
	print("button was pushed!")
	GPIO.output(10, GPIO.HIGH)

#set numbering system to BCM
GPIO.setmode(GPIO.BOARD)

#set up input channel with pull down control.
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

GPIO.setup(10, GPIO.OUT, initial=GPIO.LOW)  #set pin 10 to be an output pin and set its initial value to low

#set up event detection on pin 7 for button push rising edge
GPIO.add_event_detect(7, GPIO.RISING, callback=button_callback, bouncetime=300)


# main method
def main():

	sleep(1)

	#set up input channel without pull-up
	#GPIO.setup(7, GPIO.IN)

	#read input from gpio 7
#	input_value = GPIO.input(7)

	#set up GPIO output channel on pin 8 with initial low value
#	GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW)


#    	GPIO.setmode(GPIO.BOARD) # Use Physical pin numbering

#    	while True: #run forever
#    		if GPIO.input(7) == GPIO.HIGH:
#			print("Button was pushed!")
#			GPIO.output(10, GPIO.HIGH) #Turn on
#		sleep(1)
#		GPIO.output(8, GPIO.LOW)  #Turn off
#		sleep(1)


# Only run the functions if 
if __name__ == "__main__":
    # Make sure the GPIO is stopped correctly
    try:
        while True:
            main()
    except KeyboardInterrupt:
        print("Exiting gracefully")
        # Turn off your GPIOs here
        GPIO.cleanup()
    except Exception as e:
        GPIO.cleanup()
        print("Some other error occurred")
        print(e.message)
