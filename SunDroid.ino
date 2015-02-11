/*----------------------------------------------------
Name: SunDroid.ino
Platform: Arduino
Autor: SunDro
Date: 2015
Copyright: See license.txt file

Description:
Main file of SunDroid v0.2-Nightlie
----------------------------------------------------*/

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <Suli.h>
#include "core.h"

#define PIN_TX 7
#define PIN_RX 8
#define BAUDRATE 9600
#define SMSLENGTH 160
#define NUMBER "995598998592"
#define MESSAGE "dzudzu"

int smsIndex = 0;
char phone[16];
char date[24];
char sms[SMSLENGTH];
char gprsBuffer[64];
int inComing = 0;
int i = 0;
char *s = NULL;

/* Setup LCD Pins */
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* Set RX,TX and PWR BaudRate */
GPRS myGPRS (PIN_TX,PIN_RX,BAUDRATE);

void get_sms() 
{
	smsIndex = is_message_unread();
	if (smsIndex > 0) 
	{
		/* Recive Message and Data */
		read_message(smsIndex, sms, SMSLENGTH, phone, date);

		/* Delete old SMS' from SIM memory */
		delete_message(smsIndex);

		/* Print Data */
		lcd.print("From : ");
		lcd.println(phone);
		lcd.setCursor(0, 1);
		lcd.print("Time: ");
		lcd.println(date);
		delay(3000);

		/* Print Message */
		lcd.setCursor(0, 0);
		lcd.print("Message: ");
		lcd.setCursor(0, 1);
		lcd.println(sms);
	}
}

void setup()
{
	/* Setup LCD Display */
	pinMode(9, OUTPUT);  
	analogWrite(9, 50);   
	lcd.begin(16, 2);
	lcd.print("System is Running");

	/* Setup GPRS/GSM Serial */
	Serial.begin(9600);
	while(0 != myGPRS.init()) {
		delay(1000);
		lcd.print("Init error.");
	}
	delay(3000);
	lcd.print("Ready to recive data.");
}


void loop() 
{
  /* Check GPRS module is readable or not */
  if(myGPRS.readable()) inComing = 1;
  else delay(100);
   
  if(inComing)
  {
    /* Read GPRS module buffer */
    sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      
    /* Chek if there is incomming call and answer */
    if(NULL != strstr(gprsBuffer,"RING")) answer();
    /* Chek if there is new SMS and print */
    else if(NULL != (s = strstr(gprsBuffer,"+CMTI: \"SM\""))) get_sms();
      
    /* Send message commented because we have not physical keyboard yet */
    //send_message(NUMBER, MESSAGE);
        
    /* Make call commented because we have not physical keyboard yet */
    //make_call(NUMBER);
    
    /* Cleanup GPRS module buffer */
    sim900_clean_buffer(gprsBuffer,32);  
    inComing = 0;
    }
}
