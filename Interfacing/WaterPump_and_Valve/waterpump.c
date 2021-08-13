#include <wiringPi.h>

int main(void) // main function
{
    // Red LED: Physical pin 12, BCM GPIO18, and WiringPi pin 1.
    const int pump = 1;

    wiringPiSetup();

   pinMode(pump, OUTPUT); //setting the pin to be output

     digitalWrite(pump, HIGH); // setting the output to be HIGH

    return 0;
}
