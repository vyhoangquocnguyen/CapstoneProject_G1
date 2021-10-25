#include <stdio.h>
#include <wiringPi.h>
#include <ads1115.h>
#include <time.h>


#define AD_BASE 120


int main (void)
{

  char * Vref[]={"6.144","4.096","2.048","1.024","0.512","0.256"};
  int    rate[]={8, 16, 32, 64, 128, 250, 475, 860};
  int r,i;
  int value;

/*  from ads1115.h in /usr/include

  char * Rate=

#define ADS1115_DR_8            0
#define ADS1115_DR_16           1
#define ADS1115_DR_32           2
#define ADS1115_DR_64           3
#define ADS1115_DR_128          4
#define ADS1115_DR_250          5
#define ADS1115_DR_475          6
#define ADS1115_DR_860          7
*/


  wiringPiSetup () ;
  ads1115Setup(AD_BASE,0x48);


   for(r=0;r<8;r++)
  {
    digitalWrite(AD_BASE+1,r); // set the rate here just +1

    // let`s calulate number of scan for 2 sec
    int nb_scan =  rate[r] * 2;

    printf("Scan %4d samples at rate %3d Samples/sec for 2 sec  ...", nb_scan, rate[r]);

    time_t now = time(NULL);
    while (now == time(NULL));
    now = time(NULL);

    for(i=0;i<nb_scan;i++)
      value = analogRead(AD_BASE);

    int diff = time(NULL) - now;

    printf("---> got  %d Samples/sec \n", nb_scan / diff);
  }
  return 0 ;
}