#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp23008.h>

#define AF_BASE         100

int main (void)
{
    wiringPiSetup();
    mcp23008Setup(AF_BASE, 0x20);
    printf("Testing\n");
  //  pinMode(AF_BASE + 6, OUTPUT);
digitalWrite(AF_BASE + 1, LOW);  
digitalWrite(AF_BASE + 20, HIGH);  
digitalWrite(AF_BASE + 2, HIGH);
//  int i;
  //  for ( i = 0; i<10; ++i)
  //  pinMode (AF_BASE +i, OUTPUT);
  /* for (;;)
    {
        for(i=0;i<=10;++i)
        {
            digitalWrite(AF_BASE +i, HIGH);
            delay(500);
        }
    }*/
}
