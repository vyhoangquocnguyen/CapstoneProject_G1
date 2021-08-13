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

#include <wiringPi.h>

static BlynkTransportSocket _blynkTransport;
BlynkSocket Blynk(_blynkTransport);

#include <BlynkWidgets.h>

#define TRUE (1==1)

BLYNK_READ(10)
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
  Blynk.virtualWrite(10, DISTANCE  / 1000000) ;

}


void loop()
{
  Blynk.run();
  Blynk.read(V2);
}



int main(int argc, char* argv[])
{
  const char *auth, *serv;
  uint16_t port;
  parse_options(argc, argv, auth, serv, port);
  Blynk.begin(auth, serv, port);
  setup();
  while(1) 
  {
  loop();
  }
   return 0;
}

