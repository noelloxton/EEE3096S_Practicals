/*
 * BinClock.c
 * Jarrod Olivier
 * Modified for EEE3095S/3096S by Keegan Crankshaw
 * August 2019
 * 
 * <LXTNOE001> <BRRMAU002>
 * 21/8/2019
*/

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h> //For printf functions
#include <stdlib.h> // For system functions
#include "string.h"
#include "BinClock.h"
#include "CurrentTime.h"

//Global variables
int hours, mins, secs;
long lastInterruptTime = 0; //Used for button debounce
int RTC; //Holds the RTC instance
int var;
int test;
int HH,MM,SS;
int mins_initial = 58;
int hours_initial = 13 + TIMEZONE;
int seconds_initial = 50;
int dec_bin[4], test, test_mins, dec_bin_min[6];

void initGPIO(void){
	/* 
	 * Sets GPIO using wiringPi pins. see pinout.xyz for specific wiringPi pins
	 * You can also use "gpio readall" in the command line to get the pins
	 * Note: wiringPi does not use GPIO or board pin numbers (unless specifically set to that mode)
	 */
	printf("Setting up\n");
	wiringPiSetup(); //This is the default mode. If you want to change pinouts, be aware
	
	RTC = wiringPiI2CSetup(RTCAddr); //Set up the RTC
	
	//Set up the LEDS
	for(int i; i < sizeof(LEDS)/sizeof(LEDS[0]); i++){
	    pinMode(LEDS[i], OUTPUT);
	}
	
	//Set Up the Seconds LED for PWM
	//Write your logic here
	pinMode(SECS,PWM_OUTPUT);
	//pwmSetMode(PWM_MODE_MS);
	pwmSetRange(1024);
	pwmSetClock(59);

	
	printf("LEDS done\n");
	
	//Set up the Buttons
//	for(int j; j < sizeof(BTNS)/sizeof(BTNS[0]); j++){
		pinMode(BTNS[0], INPUT);
		pinMode(BTNS[1], INPUT);
		pullUpDnControl(BTNS[0], PUD_UP);
		pullUpDnControl(BTNS[1], PUD_UP);
//	}
/*	
	//Attach interrupts to Buttons
	for(int h; h < sizeof(BTNS)/sizeof(BTNS[0]); h++){
		wiringPiISR(BTNS[h], INT_EDGE_FALLING, myInterrupt);
	}*/
	wiringPiISR(BTNS[0],INT_EDGE_FALLING, hourInc);
	wiringPiISR(BTNS[1],INT_EDGE_FALLING, minInc);
	//Write your logic here
	
	printf("BTNS done\n");
	printf("Setup done\n");
}


/*
 * The main function
 * This function is called, and calls all relevant functions we've written
 */
int main(void){
	initGPIO();

	//Set random time (3:04PM)
	//You can comment this file out later
	wiringPiI2CWriteReg8(RTC, HOUR, decCompensation(hours_initial));
	wiringPiI2CWriteReg8(RTC, MIN, decCompensation(mins_initial));
	wiringPiI2CWriteReg8(RTC, SEC, decCompensation(seconds_initial));
	wiringPiI2CWriteReg8(RTC, SEC, 0b10000000);
	hours = hours_initial;
	mins = mins_initial;
	secs = seconds_initial;
	// Repeat this until we shut down
	for (;;){
		//Fetch the time from the RTC
		hours = hexCompensation(wiringPiI2CReadReg8(RTC,HOUR));
		mins = hexCompensation(wiringPiI2CReadReg8(RTC,MIN));
		secs = hexCompensation(wiringPiI2CReadReg8(RTC,SEC)&0b01111111);
		//Write your logic here
		
		//Function calls to toggle LEDs
		secPWM(hexCompensation(wiringPiI2CReadReg8(RTC,SEC)&0b01111111));
		lightHours();
		lightMins();
		//Write your logic here
		
		// Print out the time we have stored on our RTC
		printf("The current time is: %d:%d:%d\n", hours, mins, secs);

		//using a delay to make our program "less CPU hungry"
		delay(1000); //milliseconds
	}
	return 0;
}

//Interrupt function to print "interrupt" to console when button pushed
void myInterrupt(void){
	long interruptTime = millis();
	if(interruptTime - lastInterruptTime>200){
		printf("button push\n");
	}
	lastInterruptTime = interruptTime;
}

/*
 * Change the hour format to 12 hours
 */
int hFormat(int h){
	/*formats to 12h*/
	if (h >= 24){
		h = 0;
	}
	else if (h > 12){
		h -= 12;
	}
	return (int)h;
}

/*
 * Turns on corresponding LED's for hours
 */
void lightHours(void){
	test = hFormat(hours);
	// Write your logic to light up the hour LEDs here
	for (int k = 0; test>0; k++)
	{
		dec_bin[k]=test%2;
		test=test/2;
	}
	for (int l=0;l<=3;l++){
		digitalWrite(LEDS[l],dec_bin[l]);
		//printf("Binary Values: %d\n", dec_bin[l]);
	}
	memset(dec_bin,0, sizeof(dec_bin));
}

/*
 * Turn on the Minute LEDs
 */
void lightMins(void){
	test_mins = mins;
	for (int k = 0; test_mins>0; k++)
	{
		dec_bin_min[k]=test_mins%2;
		test_mins=test_mins/2;
	}
	for (int l=4;l<=9;l++){
		digitalWrite(LEDS[l],dec_bin_min[l-4]);
		printf("Binary Values: %d\n", dec_bin_min[l-4]);
	}
	memset(dec_bin_min,0, sizeof(dec_bin_min));
	//Write your logic to light up the minute LEDs here
}

/*
 * PWM on the Seconds LED
 * The LED should have 60 brightness levels
 * The LED should be "off" at 0 seconds, and fully bright at 59 seconds
 */
void secPWM(int units){
	// Write your logic here
	var = 1024/59;
	pwmWrite(SECS, units*var);
}

/*
 * hexCompensation
 * This function may not be necessary if you use bit-shifting rather than decimal checking for writing out time values
 */
int hexCompensation(int units){
	/*Convert HEX or BCD value to DEC where 0x45 == 0d45 
	  This was created as the lighXXX functions which determine what GPIO pin to set HIGH/LOW
	  perform operations which work in base10 and not base16 (incorrect logic) 
	*/
	int unitsU = units%0x10;

	if (units >= 0x50){
		units = 50 + unitsU;
	}
	else if (units >= 0x40){
		units = 40 + unitsU;
	}
	else if (units >= 0x30){
		units = 30 + unitsU;
	}
	else if (units >= 0x20){
		units = 20 + unitsU;
	}
	else if (units >= 0x10){
		units = 10 + unitsU;
	}
	return units;
}


/*
 * decCompensation
 * This function "undoes" hexCompensation in order to write the correct base 16 value through I2C
 */
int decCompensation(int units){
	int unitsU = units%10;

	if (units >= 50){
		units = 0x50 + unitsU;
	}
	else if (units >= 40){
		units = 0x40 + unitsU;
	}
	else if (units >= 30){
		units = 0x30 + unitsU;
	}
	else if (units >= 20){
		units = 0x20 + unitsU;
	}
	else if (units >= 10){
		units = 0x10 + unitsU;
	}
	return units;
}


/*
 * hourInc
 * Fetch the hour value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 23 hours in a day
 * Software Debouncing should be used
 */
void hourInc(void){
	//Debounce
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>150){
		printf("Interrupt 1 triggered, %d\n", hours);
		//Fetch RTC Time
		hours = hexCompensation(wiringPiI2CReadReg8(RTC, HOUR));
		//printf("Current Hour value on register is:, %x\n ",test);
		//Increase hours by 1, ensuring not to overflow
		if(hours<23){
			hours = hours + 1;
		}
		else{
			hours = 0;
		}
		//Write hours back to the RTC
		wiringPiI2CWriteReg8(RTC, HOUR,decCompensation(hours));

	}
	lastInterruptTime = interruptTime;
}

/* 
 * minInc
 * Fetch the minute value off the RTC, increase it by 1, and write back
 * Be sure to cater for there only being 60 minutes in an hour
 * Software Debouncing should be used
 */
void minInc(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>150){
		printf("Interrupt 2 triggered, %d\n",mins);
		//Fetch RTC Time
		mins = hexCompensation(wiringPiI2CReadReg8(RTC, MIN));
		//Increase minutes by 1, ensuring not to overflow
		if (mins < 59){
			mins += 1;
		}
		else{
			mins = 0;
		}
		//Write minutes back to the RTC
		wiringPiI2CWriteReg8(RTC, MIN,decCompensation(mins));
	}
	lastInterruptTime = interruptTime;
}

//This interrupt will fetch current time from another script and write it to the clock registers
//This functions will toggle a flag that is checked in main
void toggleTime(void){
	long interruptTime = millis();

	if (interruptTime - lastInterruptTime>200){
		HH = getHours();
		MM = getMins();
		SS = getSecs();

		HH = hFormat(HH);
		HH = decCompensation(HH);
		wiringPiI2CWriteReg8(RTC, HOUR, HH);

		MM = decCompensation(MM);
		wiringPiI2CWriteReg8(RTC, MIN, MM);

		SS = decCompensation(SS);
		wiringPiI2CWriteReg8(RTC, SEC, 0b10000000+SS);

	}
	lastInterruptTime = interruptTime;
}
