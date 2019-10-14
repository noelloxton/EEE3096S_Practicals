#ifndef ADC_H
#define ADC_H

//Includes
#include <wiringPi.h>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <wiringPiSPI.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <iostream>
#include "mcp3004.h"

//Define buttons I used BCM
#define START_STOP_BTN 26 // Write your value here
#define CHG_FREQ_BTN 6
#define ALARM_RST_BTN 5
#define RESET_BTN 13
#define BASE 777
#define VREF 3.3
#define VZERO 0.5
#define TC 0.01 //temperature co-efficient

//SPI Settings
#define SPI_CHAN 0 // Write your value here
#define SPI_SPEED 1350000 // adc max clock for 3.3V

//Function definitions
void readAdc(int channel);
void setup_gpio(void);
double tempConvert(int channel);
double voltageConvert(int channel);
int main(int argc,char* argv[]);

#endif
