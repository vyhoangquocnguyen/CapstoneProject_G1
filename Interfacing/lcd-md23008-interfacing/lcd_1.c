#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp23008.h>
#include <lcd.h>

//Defines for LCD
#define AF_BASE         100
#define AF_E            (AF_BASE + 2) //Enable pin
//#define AF_RW           (AF_BASE + 14) //Read/ write
#define AF_RS           (AF_BASE + 1) //Resister selec

#define AF_DB4          (AF_BASE + 3) //Data pin 4
#define AF_DB5          (AF_BASE + 4) //Data pin 5
#define AF_DB6          (AF_BASE + 5) //Data pin 6
#define AF_DB7          (AF_BASE + 6) //Data pin 7

static volatile int lcd; 
//Initialise
void lcd_init1()
{
     mcp23008Setup(AF_BASE, 0x20);
    lcd_init();
    lcd = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4, AF_DB5, AF_DB6, AF_DB7, 0, 0, 0, 0);
    
}
void lcd_input()
{
    lcdClear(lcd);
    lcdPosition(lcd,0,0);
    lcdPuts(lcd,"Hello there");
}

int main(void)
{
    wiringPiSetup():
    lcd_init1();
   lcd_input();
}