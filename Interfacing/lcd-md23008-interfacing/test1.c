#include <stdio.h>
#include <wiringPi.h>
#include <mcp23008.h>

int main (void)
{
  int i, bit ;

  wiringPiSetup () ;
  mcp23008Setup (100, 0x20) ;

  printf ("Raspberry Pi - MCP23017 Test\n") ;

  for (i = 0 ; i < 10 ; ++i)
    pinMode (100 + i, OUTPUT) ;

  pinMode         (100 + 15, INPUT) ;
  pullUpDnControl (100 + 15, PUD_UP) ;

  for (;;)
  {
    for (i = 0 ; i < 1024 ; ++i)
    {
      for (bit = 0 ; bit < 10 ; ++bit)
        digitalWrite (100 + bit, i & (1 << bit)) ;
      delay (500) ;
      while (digitalRead (100 + 15) == 1)
        delay (10) ;
    }
  }
  return 0 ;
}
