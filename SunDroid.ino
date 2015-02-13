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
#define CALL 1
#define SMS 0

int smsIndex = 0;
char phone[16];
char date[24];
char sms[SMSLENGTH];
char gprsBuffer[64];
const int btnYes = 7;
const int btnNo = 6;
int buttonState = 0;
int inComing = 0;
int i = 0;
char *s = NULL;

/* YES/NO button flags */
int yesState = 0;
int noState = 0;

/* Setup LCD Pins */
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

/* Set RX,TX and PWR BaudRate */
GPRS myGPRS (PIN_TX,PIN_RX,BAUDRATE);

/*------------------------------------
Print SMS on display                */
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

/*------------------------------------
Check buttons is pressed or not     */
int check_buttons() 
{
  yesState = digitalRead(btnYes);
  noState = digitalRead(btnNo);

  /* Check Yes button is pressed or not */
  if (yesState == HIGH) return 1;
  else return 0;
  
  /* Check NO button is pressed or not */
  if (noState == HIGH) return 2;
  else return 0;
}

/*------------------------------------
Make reaction on incoming call/sms  */
void make_reaction(int type) 
{
  /* Check if YES/NO button is pressed */
  int btnResult =  check_buttons();
  
  /* Make reaction on pressed button */
  switch(btnResult) 
  {
    case 1:
      if (type) answer();
      else get_sms();
      break;
    case 2:
      //TODO
      // Reject call function
      // Discard sms function
      break;
  }
}

/*------------------------------------
SetUp platform                      */
void setup()
{
  /* Initialize YES button as on input */
  pinMode(btnYes, INPUT);      
  /* Initialize NO button as on input */
  pinMode(btnNo, INPUT);
  
  /* Setup LCD Display */
  pinMode(9, OUTPUT);  
  analogWrite(9, 50);   
  lcd.begin(16, 2);
  lcd.print("Welcome to SunDroid");
  lcd.setCursor(0, 1);
  lcd.print("System is running");

  /* Setup GPRS/GSM Serial */
  Serial.begin(9600);
  while(0 != myGPRS.init()) {
    delay(1000);
    lcd.print("Init error.");
  }
   delay(3000);
  lcd.print("Ready to recive data.");
}

/*------------------------------------
Main Loop  */
void loop() 
{
  /* Check GPRS module is readable or not */
  if(myGPRS.readable()) inComing = 1;
  else delay(100);
  
  if(inComing)
  {
    /* Read GSM/GPRS buffer */
    sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      
    /* Chek if there is incomming call and answer or not */
    if(NULL != strstr(gprsBuffer,"RING")) make_reaction(CALL);
    /* Chek if there is new SMS and print or not */
    else if(NULL != (s = strstr(gprsBuffer,"+CMTI: \"SM\""))) make_reaction(SMS);
      
    /* Send message commented because we have not keyboard yet */
    //send_message(NUMBER, MESSAGE);
        
    /* Make call commented because we have not keyboard yet */
    //make_call(NUMBER);
    
    /* Cleanup GPRS module buffer */
    sim900_clean_buffer(gprsBuffer,32);  
    inComing = 0;
    }
}
