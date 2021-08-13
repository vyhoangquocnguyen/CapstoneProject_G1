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

// HC-SR04 ultrasonic sensor on Raspberry pi 4
#define TRIG 4
#define ECHO 5

unsigned int uptime;      		// 1 second intervals
unsigned int pinStatus;   		// status of BCM 17
unsigned int lastpinStatus = 0; // to toggle

static volatile long startTimeUsec;
static volatile long endTimeUsec;
double speedOfSoundMetersPerSecond = 340.29;


void recordPulseLength() {
    startTimeUsec = micros();
    while (digitalRead(ECHO) == HIGH);
    endTimeUsec = micros();
}

void setup()
{
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


void loop()
{
  Blynk.run();

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
  if (getCM() <= 3)
  {
    printf("Tank is full\n");
    printf("Distance: %dcm\n", getCM());
    delay(1000); // 0.5 second
  }
  else if (getCM() > 10)
  {
    printf("Tank is empty\n");
    printf("Distance: %dcm\n", getCM());
    delay(1000); // 0.5 second
  }
  else
  {
    printf("Safe zone, Distance: %dcm\n", getCM());
    delay(1000);
  }
  }
   return 0;
}

