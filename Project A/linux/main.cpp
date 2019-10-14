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
#include <ADC.h>
#include <time.h>
#define BASE 777 //defines base for the chip of the analog pins
#define VREF 3.3 //reference voltage
#define VZERO 0.5 //reference temperature voltage
#define TC 0.01 //temp -Coefficient
using namespace std;

//int buttonPin = 17; //GPIO17 Pin on the Pi
//bool buttonState = false;
BlynkTimer tmr;
int freq=1000; //freq of the monitoring of the system can = 1000, 2000, or 5000
int cnt;
int H,M,S;
int adcCh;
int firstAlarm = 1;
int reads [3];
bool alarmErr = false;
bool startStop = false; // false = stop, true = start
long lastInterruptTime = 0; //used for debouncing of button push
long lastAlarm=0;

void startStopISR(void){
    //Start stop monitoring
    long interruptTime = millis();

    if(interruptTime-lastInterruptTime>200){
        Blynk.virtualWrite(V0,"interrupt");
	if(!startStop){
	    printf("Stopped\n");
	}
	else{
	    printf("Starting\n");
	}
	startStop = !startStop; //invert the state of playing
    }
    lastInterruptTime = interruptTime;
}

void rstTime(void){
    long interruptTime = millis();
    if (interruptTime-lastInterruptTime>300){
    S=cnt-cnt;
    M=S;
    H=S;
    tmr.changeInterval(0,1000);
    Blynk.virtualWrite(V0, "clr");
    }
    lastInterruptTime=interruptTime;
}

void changeFreq(void){
    long interruptTime = millis();
     if (interruptTime - lastInterruptTime>300){
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
    cnt = getSecs();
    lastInterruptTime=interruptTime;
}

void alarmDis(void){
    //Start stop monitoring
    long interruptTime = millis();

    if(interruptTime-lastInterruptTime>200){
	alarmErr = false; //invert the state of playing
    }
    lastInterruptTime = interruptTime;
}

void setup()
{
    Blynk.begin(auth, serv,port);
    tmr.setInterval(freq, [](){
        Blynk.virtualWrite(V0, getHours(),":",getMins(),":",getSecs(),"    ",H,":",M,":",S,"\n");
    });
    //pinMode(buttonPin, INPUT); //Set GPIO17 as digital input
    //pullUpDnControl(buttonPin,PUD_UP);//set an internal pull up R for GPIO17
    mcp3004Setup(BASE, SPI_CHAN);
    wiringPiSetup();
    pinMode(12,OUTPUT);
    pinMode(26,INPUT);//button for start/stop
    pinMode(13,INPUT);//button for reset time
    pinMode(6,INPUT);//button for change frequency
    pinMode(5,INPUT);//button for dismiss alarm
    pullUpDnControl(26,PUD_UP);
    pullUpDnControl(13,PUD_UP);
    pullUpDnControl(6,PUD_UP);
    pullUpDnControl(5,PUD_UP);
    wiringPiISR(26,INT_EDGE_FALLING,startStopISR);
    wiringPiISR(13,INT_EDGE_FALLING,rstTime);
    wiringPiISR(6,INT_EDGE_FALLING,changeFreq);
    wiringPiISR(5,INT_EDGE_FALLING,alarmDis);
    cnt = getSecs();
    S = cnt-cnt;
    M = getMins()-getMins();
    H = getHours()-getHours();
}

double Vout(void){
    /*Alarm detection variable*/
    long timeNow = millis();
    double vout =1;
    vout =  (reads[1]/1023.0)*voltageConvert(0);
    if(vout<0.65 || vout>2.65){
	if(firstAlarm){
	    alarmErr = true;
	    lastAlarm = timeNow;
	    firstAlarm = 0;
	}
	else if (timeNow-lastAlarm>10000){
	    alarmErr = true; //alarm sound
	    lastAlarm = timeNow;
	}
    }
	/*else{
	    alarmErr = false;
	}
    }
    else{
	alarmErr = false; //operating stage is fine
    }*/
    //lastAlarm = timeNow;
    return vout;
}

double tempConvert(int channel){
    double Vadc = voltageConvert(channel);
    return (Vadc-VZERO)/TC;
}

double voltageConvert(int channel){
    //Voltage conversion function
    return (VREF*reads[channel])/1024;
}

void loop()
{
    Blynk.run();
    tmr.run();
    if (getSecs()==cnt+1 || (getSecs()==0 && cnt == 59))
    {
	if(S<59)
	{
	    S=S+1;
	}else{
	    if(M<59)
	    {
		M=M+1;
	        S=S-S;
	    }else{
	        if(H<23)
	        {
		    H=H+1;
		    M=M-M;
		    S=S-S;
	        }else{
		    H=H-H;
		    M=M-M;
		    S=S-S;
		}
	    }
	}
    }
    if (getSecs()==cnt+2 || (getSecs()==0 && cnt == 58)|| (getSecs()==1 && cnt==59))
    {
        if(S < 58){
	    S = S+2;
        }else{
	    if(M < 59 && S == 58){
		S = 0;
		M = M+1;
	    }else{
		    if(M<59 && S ==59){
			S = 1;
			M = M+1;
		    }
		}
	}
    }
    if (getSecs()==cnt+4 || (getSecs()==0 && cnt == 55)|| (getSecs()==1 && cnt==56) || (getSecs()== 2 && cnt== 57) || (getSecs()==3 && cnt==58) || (getSecs()==4 && cnt==59))
    {
	if(S<55){
	    S = S+5;
	}else{
	    if(M<59 && S==55){
		M = M+1;
		S = 0;
	    }else{
		if(M<59 && S==56){
		    M=M+1;
		    S=1;
		}else{
		    if(M<59 && S==57){
			M=M+1;
			S = 2;
		    }else{
			if(M<59 && S==58){
			    M=M+1;
			    S = 3;
			}else{
			    M = M+1;
			    S = 4;
			}
		    }
		}
	    }
	}
    }
    cnt = getSecs();
}

int main(int argc, char* argv[])
{
    parse_options(argc, argv, auth, serv, port);

    setup();
    Blynk.virtualWrite(V0,"clr");
    while(true) {
        loop();
	while(startStop){
	    loop();
	    for(adcCh=0; adcCh<3;adcCh++){
	        reads[adcCh] = analogRead(BASE+adcCh);
	    }
		printf("Pot: %3.2f V\t Light sen: %d \t Temp: %3.2f *C \t Vout: %3.2f\n", voltageConvert(0),reads[1],tempConvert(2),Vout());
		Blynk.virtualWrite(V3,voltageConvert(0));
		Blynk.virtualWrite(V2,tempConvert(2));
		Blynk.virtualWrite(V4,reads[1]);
		Blynk.virtualWrite(V5,Vout());
	        if(alarmErr){
	            Blynk.virtualWrite(V1,255);
	            digitalWrite(12,1);
		}else{
	    	    Blynk.virtualWrite(V1,0);
	    	    digitalWrite(12,0);
	        }

	}
	Blynk.virtualWrite(V3,"clr");
        Blynk.virtualWrite(V2,"clr");
        Blynk.virtualWrite(V4,"clr");
        Blynk.virtualWrite(V5,"clr");

    }

    return 0;
}

