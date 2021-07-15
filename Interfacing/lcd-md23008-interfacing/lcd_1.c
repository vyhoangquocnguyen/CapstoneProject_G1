#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp23008.h>
#include <lcd.h>

//Defines for LCD
#define AF_BASE         100
#define AF_E            (AF_BASE + 13) //Enable pin
#define AF_RW           (AF_BASE + 14) //Read/ write
#define AF_RS           (AF_BASE + 15) //Resister selec

#define AF_DB4          (AF_BASE + 12) //Data pin 4
#define AF_DB5          (AF_BASE + 11) //Data pin 5
#define AF_DB6          (AF_BASE + 10) //Data pin 6
#define AF_DB7          (AF_BASE +  9) //Data pin 7

static volatile int lcd; 
//Initialise
void lcd_init()
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
   lcd_input();
}