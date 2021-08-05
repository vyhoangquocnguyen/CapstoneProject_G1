// Blynk "gp" numbers are BCM numbers, so gp17 is physical pin 11 
// #define BLYNK_DEBUG
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

#include <BlynkWidgets.h>

unsigned int uptime;      		// 1 second intervals
unsigned int pinStatus;   		// status of BCM 17
unsigned int lastpinStatus = 0; // to toggle

void myTimerEvent()       		// button widget on V0 or direct access gp17 button
{
  uptime = (millis() / 1000);
  Blynk.virtualWrite(V1, uptime);
  pinStatus = digitalRead(17);
  if(pinStatus != lastpinStatus){
	lastpinStatus = pinStatus;
	printf("GP17 pin status: %i\n", pinStatus);
	if(pinStatus == 1){    // this is to synchronise V1 button if gp17 button is pressed
		Blynk.virtualWrite(V0, 1);
	}
	else{
		Blynk.virtualWrite(V0, 0);
	}
  }
}

void setup()
{
  //nothing to go here yet
}

BLYNK_WRITE(V0)  // button set at PUSH frequency
{
  if(param[0] == 1){
	printf("V1 turned device ON\n");
	digitalWrite (17, HIGH) ;  
  }
  else{
	printf("V1 turned device OFF\n");
	digitalWrite (17, LOW) ;
  }
}

void loop()
{
  Blynk.run();
  if(millis() >= uptime + 1){  // 1 second intervals
	myTimerEvent();
  }
}

int main(int argc, char* argv[])
{
    const char *auth, *serv;
    uint16_t port;
    parse_options(argc, argv, auth, serv, port);
    Blynk.begin(auth, serv, port);
    while(true) {
		loop();
    }
    return 0;
}