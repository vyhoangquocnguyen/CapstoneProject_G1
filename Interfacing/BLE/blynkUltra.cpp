/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  This example shows how value can be pushed from Arduino to
  the Blynk App.

  NOTE:
  BlynkTimer provides SimpleTimer functionality:
    http://playground.arduino.cc/Code/SimpleTimer

  App project setup:
    Value Display widget attached to Virtual Pin V5
 *************************************************************/

/* Comment this out to disable prints and save space */
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
#include <sys/time.h>

BlynkTimer timer;

int distance()
{    
// Define GPIO to use on Pi ( wiringPI numbers )
int GPIO_TRIGGER=4 ;
int GPIO_ECHO=5 ;

//Compensate distance from edge to sensor
int COMP_SENSOR=0 ;

//Variables used by gettimeofday
struct timeval start, stop ; 

// Set pins as output and input
wiringPiSetup () ;
  pinMode (GPIO_TRIGGER, OUTPUT) ;
  pinMode (GPIO_ECHO, INPUT) ;

//Set trigger to False (Low)
digitalWrite (GPIO_TRIGGER,  0) ;

//200 ms delay to start 
delay (200) ;

//Send 10us pulse to trigger
digitalWrite (GPIO_TRIGGER,  1) ;
delayMicroseconds (10) ;
digitalWrite (GPIO_TRIGGER,  0) ;
gettimeofday(&start, NULL) ;
while (digitalRead (GPIO_ECHO)==0) 
{
gettimeofday(&start, NULL) ;
} 
while (digitalRead (GPIO_ECHO)==1) 
{
gettimeofday(&stop, NULL) ;
} 

// Calculate pulse length
float TIME_DELTA, DISTANCE ;
TIME_DELTA=(stop.tv_sec-start.tv_sec) + (stop.tv_usec-start.tv_usec) ;

// Distance pulse travelled in that time is time multiplied by the speed of sound (cm/s)
DISTANCE=TIME_DELTA * 34000 ;

// That was the distance there and back so halve the value
DISTANCE=DISTANCE / 2 ;

// Add compensation
DISTANCE=DISTANCE + COMP_SENSOR ;

// This command writes result to Virtual Pin (10) and convert usec to sec 
//Blynk.virtualWrite(V2, DISTANCE  / 1000000) ;

return DISTANCE;
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5,distance());
  delay(1000);
  
  //distance();
}

void setup()
{
  // Setup a function to be called every second
  timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

int main(int argc, char* argv[])
{
  const char *auth, *serv;
  uint16_t port;
  parse_options(argc, argv, auth, serv, port);

  Blynk.begin(auth, serv, port);

  setup();
  while(true) {
    loop();
  }

  return 0;
}
