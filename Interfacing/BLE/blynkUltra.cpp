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

BlynkTimer timer;
#define TRUE (1==1)

// HC-SR04 ultrasonic sensor on Raspberry pi 4
#define TRIG 4
#define ECHO 5

static volatile long startTimeUsec;
static volatile long endTimeUsec;
double speedOfSoundMetersPerSecond = 340.29;

void recordPulseLength() {
    startTimeUsec = micros();
    while (digitalRead(ECHO) == HIGH);
    endTimeUsec = micros();
}

void setupUltrasonic() {
    wiringPiSetup();
    pinMode(TRIG, OUTPUT);
    pinMode(ECHO, INPUT);

    // TRIG pin must start LOW
    // Initialize the sensor's trigger pin to low. If we don't pause
    // after setting it to low, sometimes the sensor doesn't work right.
    digitalWrite(TRIG, LOW);
    delay(500); // .5 seconds
}

int getCM() {
    // Send trig pulse
    // Triggering the sensor for 10 microseconds will cause it to send out
    // 8 ultrasonic (40Khz) bursts and listen for the echos.
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    int now = micros();
    // Wait for echo start
    // The sensor will raise the echo pin high for the length of time that it took
    // the ultrasonic bursts to travel round trip.
    while (digitalRead(ECHO) == LOW && micros() - now < 30000);
    recordPulseLength();
    long travelTimeUsec = endTimeUsec - startTimeUsec;
    double distanceMeters = 100 * ((travelTimeUsec / 1000000.0) * 340.29) / 2;
    return distanceMeters ;
}

// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V5, millis() / 1000);
  Blynk.vỉtualWrite(V2, getCM));
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
