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

# Logic that you write
def main():
	GPIO.setwarnings(False)  # Ignore Warnings for now
    	GPIO.setmode(GPIO.BOARD) # Use Physical pin numbering
    	GPIO.setup(8, GPIO.OUT, initial=GPIO.LOW)  #set pin 8 to be an output pin and set its initial value to low
   
    	while True: #run forever
    		GPIO.output(8, GPIO.HIGH) #Turn on
		sleep(1)
		GPIO.output(8, GPIO.LOW)  #Turn off
		sleep(1)


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
