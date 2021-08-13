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

void blynkUltra()
{
    int distance = getCM();
    Blynk.virtualWrite(V2, distance);

}

void setup()
{
  setupUltrasonic();

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
    blynkUltra();
  }
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

