
#include <iostream>

extern "C" {

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <wiringPi.h>
#include <malloc.h>
#include <stdlib.h>
#include <wiringPiI2C.h>
#include <pthread.h>
#include <sched.h>  // Scheduling library for prioritization of threads
}

using namespace std;

int g, s, x, y;


// Define some device parameters
#define I2C_ADDR  0x20//  0x27 // I2C device address

// Define some device constants
#define LCD_CHR  1 // Mode - Sending data
#define LCD_CMD  0 // Mode - Sending command

#define LINE1  0x80 // 1st line
#define LINE2  0xC0 // 2nd line

#define LCD_BACKLIGHT   0x08  // On
// LCD_BACKLIGHT = 0x00  # Off

#define ENABLE  0b00000100 // Enable bit

void lcd_init(void);
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);

// added by Lewis
void typeInt(int i);
void typeFloat(float myFloat);
void lcdLoc(int line); //move cursor
void ClrLcd(void); // clr LCD return home
void typeln(const char *s);
void typeChar(char val);
int fd, fdl;  // seen by all subroutines

int tmp = 0;
int hum = 0;
int moist = 0;
int us = 0;

const int lightPin = 0; //GPIO 17
const int pumpPin = 2; //GPIO 27


void *checkMoisture(void *arg)
{
  FILE *fp;
  char ch[1];
  int pumpflag = 2;
  while(1)
  {
    
    fp=fopen("/home/pi/data.txt","r");
    fscanf(fp,"%[^\n]",ch);
    fflush(fp);
    fclose(fp);
    
    if ((moist > 450 || strcmp(ch,"1") == 0 ) && (pumpflag == 0 || pumpflag == 2))
    {
      digitalWrite (pumpPin, HIGH) ; 
      printf("Water Pump ON.\n");
      pumpflag = 1;
      delay(1000);
    }
    if(moist < 430 && (pumpflag == 1 || pumpflag == 2))//|| strcmp(ch,"0") == 0)
    {
      digitalWrite (pumpPin,  LOW) ;
      printf("Water Pump OFF.\n");
      pumpflag = 0;
    }
    delay(1);
  }
  return 0;
} 

void *checkTemp(void *arg)
{
  int lightflag = 2;
  
  FILE *fp2;
  char ch2[1];
  while(1)
  {
    
    fp2=fopen("/home/pi/data2.txt","r");
    fscanf(fp2,"%[^\n]",ch2);
    fflush(fp2);
    fclose(fp2);
    
    
    if ((tmp < 90 || strcmp(ch2,"1") == 0) && (lightflag == 0 || lightflag == 2))
    {
      digitalWrite (lightPin, HIGH) ; 
      printf("Lights ON.\n");
      lightflag = 1;
      delay(1000);
    }
    else if (tmp >= 90 && (lightflag == 1 || lightflag == 2))
    {
      digitalWrite (lightPin,  LOW) ;
      printf("Lights OFF.\n");
      lightflag = 0;
    }
    delay(1);
  }
  return 0;
}  





int main ()
{
  
  printf("Code  Started \n");
  
	if(wiringPiSetup() == -1)
		return 1;
	
  printf("Setting  Up  GPIO  Pins \n");
  pinMode (lightPin, OUTPUT) ;
  pinMode (pumpPin, OUTPUT) ;
  
  
  printf("Connectiong to LCD \n");
	fdl = wiringPiI2CSetup(I2C_ADDR);
  
  
  printf("Initializing LCD \n");
	lcd_init(); // setup LCD
	lcdLoc(LINE1);
	typeln("SMART GREENIFY");
	lcdLoc(LINE2);
	typeln("DEVICE");

  printf("Trying  Connecting to  Arduino \n");
  
  if ((fd = serialOpen ("/dev/ttyUSB0", 9600)) < 0)
  {
    fprintf (stderr, "Unable to open USB0 serial device: %s\n", strerror (errno)) ;
    if ((fd = serialOpen ("/dev/ttyUSB1", 9600)) < 0)
    {
      fprintf (stderr, "Unable to open USB1 serial device: %s\n", strerror (errno)) ;
      if ((fd = serialOpen ("/dev/ttyACM0", 9600)) < 0)
      {
        fprintf (stderr, "Unable to open USB2 serial device: %s\n MAKE SURE ARDUINO IS CONNECTED.\n", strerror (errno)) ;
      }
    }
    
  }
  
  serialFlush(fd);
  

int available = 0;

printf("Starting the Streaming \n");
system("/usr/bin/python3 /home/pi/streamingTest.py &");
printf("\n  Video Streaming GStarted at: ");
system("hostname -I");
printf("   port: 8000 \n");

printf("Starting  Google Sheets,  Blynk & Music \n");
system("/usr/bin/python3 /home/pi/google_sheet.py &");

delay(5000);

  pthread_attr_t moisture_thread_attr, temp_thread_attr;
  pthread_t moisture_thread,temp_thread;
  sched_param param1,param2;
  
  // pthreads for RTOS operation 
  pthread_attr_init (&moisture_thread_attr);
  pthread_attr_init (&temp_thread_attr);
  
  pthread_attr_getschedparam (&moisture_thread_attr, &param1);
  pthread_attr_getschedparam (&temp_thread_attr, &param2);
  
  param1.sched_priority = 1;
  pthread_attr_setschedparam (&moisture_thread_attr, &param1);

  param2.sched_priority = 2;
  pthread_attr_setschedparam (&temp_thread_attr, &param2);
  
  
  pthread_create(&moisture_thread, &moisture_thread_attr, &checkMoisture, NULL);
  pthread_create(&temp_thread, &temp_thread_attr, &checkTemp, NULL);
    

  while(1)
  {
    char data[500];
    printf("Checking Available Serial Data... ");
    available = serialDataAvail(fd);
    printf(" Available Characters: %d\n", available);
    if (available > 95)
    {
      serialFlush(fd);
      continue;
    }
    
   
    
    
    
    if (available > 35)
    {
      printf("GOT DATA:");
      for(int i=0; i < available; i++)
      {
        data[i] = serialGetchar(fd);
      }
      
      
      for(int j = 0; j < available; j++)
      {
        printf("%c",data[j]);
      }
      
      printf("\n");
      FILE *fr;
      fr=fopen("/home/pi/data_log.txt","w");
      fprintf(fr,"%s",data);
      fflush(fr);
      fclose(fr);
      
      int t = strstr(data, "TEMP:")-data;
      int h = strstr(data, "HUM:")-data;
      int m = strstr(data, "MOIST:")-data;
      int u = strstr(data, "US:")-data;
      
      
      char temp [5];
      strncpy(temp,data+t+5,4);
      printf("\nTEMP VAL: ");
      tmp = atoi(temp);
      printf("%d \n",tmp);
      
      
      
      char hm [5];
      strncpy(hm,data+h+4,4);
      printf("\nHUM VAL: ");
      hum = atoi(hm);
      printf("%d \n",hum);
      
      
      
      char mst [5];
      strncpy(mst,data+m+6,4);
      printf("\nMOIST VAL: ");
      moist = atoi(mst);
      printf("%d \n",moist);
      
      
      
      char uls [5];
      strncpy(uls,data+u+3,4);
      printf("\nULTRASONIC VAL: ");
      us = atoi(uls);
      printf("%d \n",us);
      
      printf(" \n");
      
      //ClrLcd();
      
      lcdLoc(LINE1);
      typeln("Temp:");
      typeInt(tmp);
      
      typeln("F Hum:");
      typeInt(hum);
      typeln("   ");
      
      lcdLoc(LINE2);
      typeln("Mois:");
      
      if(moist > 450) typeln("LO");
      else if(moist > 250) typeln("OK");
      else typeln("HI");
      
      typeln(" Tank:");
      if (us > 20) typeln("LO  ");
      else if(us > 10) typeln("OK  ");
      else typeln("FL  ");
      
      
      //checkTemp();
      //checkMoisture();
      
      
      delay(200);

      
    }
    else
    {
      printf("Serial Data not yet Ready.\n");
      delay(200);
      
    }    
  }
  
}  

                    

// float to string
void typeFloat(float myFloat)   {
  char buffer[20];
  sprintf(buffer, "%4.2f",  myFloat);
  typeln(buffer);
}

// int to string
void typeInt(int i)   {
  char array1[20];
  sprintf(array1, "%d",  i);
  typeln(array1);
}

// clr lcd go home loc 0x80
void ClrLcd(void)   {
  lcd_byte(0x01, LCD_CMD);
  lcd_byte(0x02, LCD_CMD);
}

// go to location on LCD
void lcdLoc(int line)   { lcd_byte(line, LCD_CMD); }

// out char to LCD at current position
void typeChar(char val)   { lcd_byte(val, LCD_CHR); }


// this allows use of any size string
void typeln(const char *s)   { while ( *s ) lcd_byte(*(s++), LCD_CHR); }

void lcd_byte(int bits, int mode)   {

  //Send byte to data pins
  // bits = the data
  // mode = 1 for data, 0 for command
  int bits_high;
  int bits_low;
  // uses the two half byte writes to LCD
  bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT ;
  bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT ;

  // High bits
  wiringPiI2CReadReg8(fdl, bits_high);
  lcd_toggle_enable(bits_high);

  // Low bits
  wiringPiI2CReadReg8(fdl, bits_low);
  lcd_toggle_enable(bits_low);
}

void lcd_toggle_enable(int bits)   {
  // Toggle enable pin on LCD display
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fdl, (bits | ENABLE));
  delayMicroseconds(500);
  wiringPiI2CReadReg8(fdl, (bits & ~ENABLE));
  delayMicroseconds(500);
}


void lcd_init()   {
  // Initialise display
  lcd_byte(0x33, LCD_CMD); // Initialise
  lcd_byte(0x32, LCD_CMD); // Initialise
  lcd_byte(0x06, LCD_CMD); // Cursor move direction
  lcd_byte(0x0C, LCD_CMD); // 0x0F On, Blink Off
  lcd_byte(0x28, LCD_CMD); // Data length, number of lines, font size
  lcd_byte(0x01, LCD_CMD); // Clear display
  delayMicroseconds(500);
}
