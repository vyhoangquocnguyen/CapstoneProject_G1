#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <mcp23008.h>
#include <lcd.h>

//Defines for LCD
#define AF_BASE         100
#define AF_E            (AF_BASE + 2) //Enable pin

#define AF_RS           (AF_BASE + 1) //Resister selec

#define AF_DB4          (AF_BASE + 3) //Data pin 4
#define AF_DB5          (AF_BASE + 4) //Data pin 5
#define AF_DB6          (AF_BASE + 5) //Data pin 6
#define AF_DB7          (AF_BASE + 6) //Data pin 7

static volatile int lcd;
static volatile float temp; 
static volatile int humid;
//Initialise
void lcd_setup()
{
     mcp23008Setup(AF_BASE, 0x20);
    lcd_init();
    lcd = lcdInit (2, 16, 4, AF_RS, AF_E, AF_DB4, AF_DB5, AF_DB6, AF_DB7, 0, 0, 0, 0);
    
}
int lcd_input(int msg)
{
    lcdClear(lcd);
    
    switch(msg)
    {
        case 1:
            lcdClear(lcd);
            lcdPosition(lcd,0,0);
            lcdPuts(lcd,"Temperature: %f C", temp);
            lcdPosition(lcd,0,1);
            lcdPuts(lcd,"Humidity: %d %", humid);
        break;

        case 2:
            lcdClear(lcd);
            lcdPosition(lcd,0,0);
            lcdPuts(lcd,"Water level");
        break;

        default:

    }
    
    lcdPuts(lcd,"Hello there");
}

int main(void)
{
    
    wiringPiSetup():
    lcd_setup();
    lcd_input();

}