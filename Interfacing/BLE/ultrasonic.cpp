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
//#include <WiringPi.h>
#include <sys/time.h>

// Define GPIO to use on Pi ( wiringPI numbers )
#define GPIO_TRIGGER=4 ;
#define GPIO_ECHO=5 ;
//Variables used by gettimeofday
struct timeval start, stop ; 
// Calculate pulse length
float TIME_DELTA, DISTANCE ;

/***Functions****/

void ultrsetup()
{
// Set pins as output and input
    wiringPiSetup () ;
    pinMode (GPIO_TRIGGER, OUTPUT) ;
    pinMode (GPIO_ECHO, INPUT) ;
    //Set trigger to False (Low)
    digitalWrite (GPIO_TRIGGER,  0) ;
    //200 ms delay to start 
    delay (200) ;
}
void level()
{
    //Send 10us pulse to trigger
    digitalWrite (GPIO_TRIGGER,  1) ;
    delayMicroseconds (10) ;
    digitalWrite (GPIO_TRIGGER,  0) ;
    gettimeofday(&start, NULL) ;
    //reading sensor
    while (digitalRead (GPIO_ECHO)==0) 
    {
        gettimeofday(&start, NULL) ;
    } 
    while (digitalRead (GPIO_ECHO)==1) 
    {
        gettimeofday(&stop, NULL) ;
        TIME_DELTA=(stop.tv_sec-start.tv_sec) + (stop.tv_usec-start.tv_usec) ;
        // Distance pulse travelled in that time is time multiplied by the speed of sound (cm/s)
        DISTANCE=TIME_DELTA * 34000 ;
        // That was the distance there and back so halve the value
        DISTANCE=DISTANCE / 2 ;
        // This command writes result to Virtual Pin (10) and convert usec to sec 
        Blynk.virtualWrite(10, DISTANCE  / 1000000) ;
    }

}

void setup()
{
    Blynk.begin(auth, serv, port);
    ultrsetup();
}

void loop()
{
    Blynk.run();
    level();
}

int main(int argc, char* argv[])
{
    parse_options(argc, argv, auth, serv, port);

    setup();
    while(true) 
    {
        loop();
    }

    return 0;
}

//sudo ./blynk --token=qlxUAUH_0vIwxvcb2W06yIsTKeE5c281