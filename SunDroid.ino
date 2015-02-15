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
#include <TFT.h>
#include <SPI.h>
#include <Wire.h>
#include <Suli.h>
#include "core.h"

/* Define pins for LCD Display */
#define CS 10
#define DC 9
#define RST 6

/* Define pins for GSM/GPRS Shield */
#define PIN_TX 7
#define PIN_RX 8
#define BAUDRATE 9600
#define SMSLENGTH 160

/* Define pins for CALL/SMS Functions */
#define NUMBER "995598998592"
#define MESSAGE "dzudzu"
#define CALL 1
#define SMS 0

/* Used Variables */
const int btnYes = 1;
const int btnNo = 0;
int smsIndex = 0;
int inComing = 0;
int i = 0;
char phone[16];
char date[24];
char sms[SMSLENGTH];
char gprsBuffer[64];
char *s = NULL;

/* YES/NO button flags */
int yesState = 0;
int noState = 0;

/* Setup LCD pins */
TFT TFTscreen = TFT(CS, DC, RST);

/* Set RX,TX and PWR BaudRate */
GPRS m_gprs (PIN_TX,PIN_RX,BAUDRATE);

/*------------------------------------
Print SMS on display                */
void get_sms() 
{
  smsIndex = m_gprs.is_message_unread();
  if (smsIndex > 0) 
    {
      /* Recive Message and Data */
      m_gprs.read_message(smsIndex, sms, SMSLENGTH, phone, date);

      /* Delete old SMS' from SIM memory */
      m_gprs.delete_message(smsIndex);

       /* Print Message */
      TFTscreen.stroke(255,255,255);
      TFTscreen.setTextSize(1);
      TFTscreen.text("Message\n ",0,50);
      TFTscreen.text(sms,0,60);
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
      if (type) m_gprs.answer();
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
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255,255,255);
  TFTscreen.setTextSize(1);
  TFTscreen.text("Welcome to SunDroid v0.2!\n ",0,0);

  /* Setup GPRS/GSM Serial */
  Serial.begin(9600);
  while(m_gprs.init()) {
    delay(1000);
    TFTscreen.text("SunDroid Is Running!\n ",0,0);
    TFTscreen.text("GSM/GPRS Init Error! Wait...\n ",0,20);
  }
  delay(3000);
  TFTscreen.background(0, 0, 0);
  TFTscreen.text("SunDroid Is Running!\n ",0,0);
  TFTscreen.text("SunDrid is ready!\n ",0,20);
}

/*------------------------------------
Main Loop  */
void loop() 
{
  /* Setup Display */
  TFTscreen.stroke(255,255,255);
  TFTscreen.setTextSize(1);
  
  /* Check GPRS module is readable or not */
  if(m_gprs.readable()) inComing = 1;
  else delay(100);

  if(inComing)
  {
    /* Read GSM/GPRS buffer */
    sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
      
    /* Chek if there is incomming call and answer or not */
    if(NULL != strstr(gprsBuffer,"RING")) {
      TFTscreen.text("Incomming Call\n ",0,20);
      TFTscreen.text(phone,0,30);
      make_reaction(CALL);
    }
    
    /* Chek if there is new SMS and print or not */
    else if(NULL != (s = strstr(gprsBuffer,"+CMTI: \"SM\""))) {
      TFTscreen.text("New Message From: \n ",0,30);
      TFTscreen.text(phone,0,30);
      TFTscreen.text("Time: \n ",0,40);
      TFTscreen.text(date,0,50);
      make_reaction(SMS);
    }

    /* Send message commented because we have not keyboard yet */
    //send_message(NUMBER, MESSAGE);
        
    /* Make call commented because we have not keyboard yet */
    //make_call(NUMBER);
    
    /* Cleanup GPRS module buffer */
    sim900_clean_buffer(gprsBuffer,32);  
    inComing = 0;
    }
}
