/**
 * @file       main.cpp
 * @author     Noel Loxton & Mauro Borrageiro
 Modified from code originally written by Volodymyr Shymanskyy for Blynk 
 * @date       13/10/2019
 */

//#define BLYNK_DEBUG
#define BLYNK_PRINT stdout
#ifdef RASPBERRY
  #include <BlynkApiWiringPi.h>
#else
  #include <BlynkApiLinux.h>
#endif
#include <BlynkSocket.h>
#include <BlynkOptionsParser.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

static const char *auth, *serv;
static uint16_t port;

#include <BlynkWidgets.h>
#include <CurrentTime.c>
#include <wiringPi.h>

int buttonPin = 17; //GPIO17 Pin on the Pi
bool buttonState = false;
BlynkTimer tmr;
//BlynkTimer tmr2;
//BlynkTimer tmr5;
int freq=1000; //freq of the monitoring of the system can = 1000, 2000, or 5000
long lastInterruptTime1 = 0; //used for debouncing of terminal printing
long lastInterruptTime2 = 0; //used for debouncing of button push

/*void setFreq(){
    long interruptTime = millis();
    if (interruptTime-lastInterruptTime1>150){
        tmr.setInterval(freq, [](){
           Blynk.virtualWrite(V1, getHours(),":",getMins(),":",getSecs(),"\n");
        });
    }
   lastInterruptTime1=interruptTime;
}*/

void changeFreq(void){
    long interruptTime = millis();
     if (interruptTime - lastInterruptTime2>300){
	if (freq==1000){
 	    freq = 2000;
	    tmr.changeInterval(0,freq);
	}else{
	    if(freq==2000){
	    freq = 5000;
	    tmr.changeInterval(0,freq);
	    }else{
	    freq = 1000;
	    tmr.changeInterval(0,freq);
	    }
	}

     }
lastInterruptTime2=interruptTime;
}

void setup()
{
    Blynk.begin(auth, serv, port);
    tmr.setInterval(freq, [](){
        Blynk.virtualWrite(V1, getHours(),":",getMins(),":",getSecs(),"\n");
    });
    pinMode(buttonPin, INPUT); //Set GPIO17 as digital input
    pullUpDnControl(buttonPin,PUD_UP);//set an internal pull up R for GPIO17
    wiringPiSetup();
    pinMode(27,INPUT);
    pullUpDnControl(27,PUD_UP);
    wiringPiISR(27,INT_EDGE_FALLING,changeFreq);
    //Blynk.virtualWrite(V1, "clr");
}

void loop()
{
  
    //setFreq();
    Blynk.run();
    //setFreq();
    tmr.run();
}

int main(int argc, char* argv[])
{
    parse_options(argc, argv, auth, serv, port);

    setup();
    while(true) {
        loop();
//	setFreq();
    }

    return 0;
}

