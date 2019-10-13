/**
 * @file       main.cpp
 * @author     Volodymyr Shymanskyy
 * @license    This project is released under the MIT License (MIT)
 * @copyright  Copyright (c) 2015 Volodymyr Shymanskyy
 * @date       Mar 2015
 * @brief
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

int buttonPin = 17; //GPIO17 Pin on the Pi
bool buttonState = false;
BlynkTimer tmr;

BLYNK_WRITE(V2)
{
    printf("Got a value: %s\n", param[0].asStr());
}

void setup()
{
    Blynk.begin(auth, serv, port);
    tmr.setInterval(5000, [](){
      Blynk.virtualWrite(V1, getHours(),":",getMins(),":",getSecs(),"\n");
    });
    pinMode(buttonPin, INPUT); //Set GPIO17 as digital input
    pullUpDnControl(buttonPin,PUD_UP);//set an interanl pull up R for GPIO17
}

void loop()
{
    Blynk.run();
    tmr.run();
    if(buttonState != digitalRead(buttonPin)) //check the button state against its last known value, if true:
    {
       if(digitalRead(buttonPin) == TRUE) //if true, set the Virtual Pin "V0" to a value of 0 (full off)
        {
            Blynk.virtualWrite(V0, 0); 
        }
        else{  
            Blynk.virtualWrite(V0, 255);  //Else we set the virtual pin "V0" to a value of 255 (full on)
        }
    }   
    else {}    //if last value = current value, we do nothing. 
    buttonState = digitalRead(buttonPin);  //update the button state. 
}


int main(int argc, char* argv[])
{
    parse_options(argc, argv, auth, serv, port);

    setup();
    while(true) {
        loop();
    }

    return 0;
}

