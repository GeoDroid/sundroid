/*----------------------------------------------------
Name: core.h
Platform: Arduino
Autor: SunDro
Date: 2015
Copyright: See license.txt file

Description:
Header file for library of SunDroid v0.2-Nightlie
----------------------------------------------------*/

#ifndef __GPRS_SHIELD_ARDUINO_H__
#define __GPRS_SHIELD_ARDUINO_H__

#include <sim900_suli.h>

/* Message Length */
#define MESSAGE_LENGTH  20
    
/* Send message */
int send_message(char* num, char* data);

/* Check if there is unread sms */
int is_message_unread();
    
/* Read sms & change status to readed */
int read_message(int smsIndex, char *msg, int length, char *phone, char *date);

/* Read Messages */
int read_message(int smsIndex, char *msg, int length);

/* Delete messages on sim card */
int delete_message(int index);

/* Function for call */
int make_call(char* num);

/* Auto answer for incoming call */
int answer(void);

/* Check sim status */
int sim_status(void);
 
class GPRS
{
public:
    /* Create GPRS instance */
    GPRS(int tx, int rx, uint32_t baudRate = 9600, const char* apn = NULL, const char* userName = NULL, const char *passWord = NULL);
    
    /* get instance of GPRS class */
    static GPRS* getInstance() {
        return inst;
    };
    
    /* initialize GPRS module including SIM card check & signal strength */
    int init(void);

    /* check if GPRS module is readable or not */
    int readable(void);

    /** wait a few time to check if gprs module is readable or not */
    int wait_readable(int wait_time);
    
private:
    SoftwareSerial gprsSerial;
    PIN_T _powerPin;
    static GPRS* inst;
    const char* pass;
    const char* m_apn;
    const char* user;
};

#endif
