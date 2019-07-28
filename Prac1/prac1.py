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
import itertools

#make array variable storing itertools.product
#global i[] = itertools.product([0,1],repeat=3)
#global int x = 0

#button1/black_button detection method
def button1_increment(channel):
	print("black button was pushed!")
	print("incrementing...\n")
	GPIO.output(10, GPIO.HIGH)
	GPIO.output(11, GPIO.LOW)
#button2/red_button detection method
def button2_decrement(channel):
	print("red button was pushed!")
	print("decrementing...\n")
	GPIO.output(10, GPIO.LOW)
	GPIO.output(11, GPIO.HIGH)

#set numbering system to BCM
GPIO.setmode(GPIO.BOARD)

#ignore warning for now
GPIO.setwarnings(False)

#set up pin 7 to be an input channel with pull down control.
GPIO.setup(7, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

#set up pin 8 to be an input channel with pull down control
GPIO.setup(8, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

#set pin 10 to be an output pin and set its initial value to low
GPIO.setup(10, GPIO.OUT, initial=GPIO.LOW)

#set pin 11 to be an output pin and set its initial value to low
GPIO.setup(11, GPIO.OUT, initial=GPIO.LOW)

#set pin 12 to be an output pin and set its initial value to low
GPIO.setup(12, GPIO.OUT, initial=GPIO.LOW)

#set up event detection on pin 7 for button push rising edge
GPIO.add_event_detect(7, GPIO.RISING, callback=button1_increment, bouncetime=300)

#set up event detection on pin 8 for button push rising edge
GPIO.add_event_detect(8, GPIO.RISING, callback=button2_decrement, bouncetime=300)

# main method
def main():

	sleep(1)  #process to wait for button push

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
