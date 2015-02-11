/*----------------------------------------------------
Name: core.cpp
Platform: Arduino
Autor: SunDro
Date: 2015
Copyright: See license.txt file

Description:
Library of SunDroid v0.2-Nightlie
----------------------------------------------------*/

#include <stdio.h>
#include <Suli.h>
#include "core.h"

GPRS* GPRS::inst;

/*----------------------------------------------------
Create GPRS instance                                */
GPRS::GPRS(PIN_T tx, PIN_T rx, uint32_t baudRate, const char* apn, const char* userName, const char* passWord):gprsSerial(tx,rx)
{
    inst = this;
    m_apn = apn;
    user = userName;
    pass = passWord;
    sim900_init(&gprsSerial, -1, baudRate);
}

/*----------------------------------------------------
initialize GPRS module                              */
int GPRS::init(void)
{
    if(sim900_check_with_cmd("AT\r\n","OK",DEFAULT_TIMEOUT,CMD)) return -1;
    if(sim900_check_with_cmd("AT+CFUN=1\r\n","OK",DEFAULT_TIMEOUT,CMD)) return -1;
    if(sim_status()) return -1;

    return 0;
}

/*----------------------------------------------------
Check sim card satus                                */
int sim_status(void)
{
    char gprsBuffer[32];
    int count = 0;

    sim900_clean_buffer(gprsBuffer,32);
    while(count < 3) 
    {
        sim900_send_cmd("AT+CPIN?\r\n");
        sim900_read_buffer(gprsBuffer,32,DEFAULT_TIMEOUT);
        if((NULL != strstr(gprsBuffer,"+CPIN: READY"))) break;
        count++;
        suli_delay_ms(300);
    }
    if(count == 3) return -1;

    return 0;
}

/*----------------------------------------------------
Send message                                        */
int send_message(char *num, char *data)
{
    char cmd[32];

    if(0 != sim900_check_with_cmd("AT+CMGF=1\r\n", "OK", DEFAULT_TIMEOUT,CMD))  return -1;
    suli_delay_ms(500);
    snprintf(cmd, sizeof(cmd),"AT+CMGS=\"%s\"\r\n", num);
    if(0 != sim900_check_with_cmd(cmd,">",DEFAULT_TIMEOUT,CMD)) return -1;

    suli_delay_ms(1000);
    sim900_send_cmd(data);
    suli_delay_ms(500);
    sim900_send_End_Mark();

    return 0;
}

/*----------------------------------------------------
Check message is unread or not                      */
int is_message_unread()
{
    char gprsBuffer[64];
    char *p,*s;
    
    /* List of unread sms */
    sim900_send_cmd("AT+CMGL=\"REC UNREAD\",1\r\n");
    sim900_clean_buffer(gprsBuffer,64);
    sim900_read_buffer(gprsBuffer,64,DEFAULT_TIMEOUT); 
    
    if(NULL != (s = strstr(gprsBuffer,"OK"))) return 0;
    else 
    {
        if(NULL != (s = strstr(gprsBuffer,"+CMGL:"))) 
        {
            p = strstr((char *)gprsBuffer,":");
            if (p != NULL) 
            {
                sim900_wait_for_resp("OK", DEFAULT_TIMEOUT, CMD);
                return atoi(p+1);
            }
        } 
        else return 0; 
    } 
    return -1;
}

/*----------------------------------------------------
Read message and change status to readed            */
int read_message(int smsIndex, char *msg, int length, char *phone, char *date)  
{
    int i = 0;
    char gprsBuffer[100 + length];
    char cmd[16];
    char *p,*p2,*s;
    
    sim900_check_with_cmd("AT+CMGF=1\r\n","OK",DEFAULT_TIMEOUT,CMD);
    suli_delay_ms(1000);
    sprintf(cmd,"AT+CMGR=%d\r\n",smsIndex);
    sim900_send_cmd(cmd);
    sim900_clean_buffer(gprsBuffer,sizeof(gprsBuffer));
    sim900_read_buffer(gprsBuffer,sizeof(gprsBuffer),DEFAULT_TIMEOUT);
    
    if(NULL != ( s = strstr(gprsBuffer,"+CMGR:")))
    {
        p = strstr(s,",");
        p2 = p + 2;
        p = strstr((char *)(p2), "\"");
        if (NULL != p) 
        {
            i = 0;
            while (p2 < p) phone[i++] = *(p2++);
            phone[i] = '\0';            
        }
        p = strstr((char *)(p2),",");
        p2 = p + 1; 
        p = strstr((char *)(p2), ","); 
        p2 = p + 2;
        p = strstr((char *)(p2), "\"");
        if (NULL != p) 
        {
            i = 0;
            while (p2 < p) date[i++] = *(p2++);
            date[i] = '\0';
        }        
        if(NULL != ( s = strstr(s,"\r\n")))
        {
            i = 0;
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) msg[i++] = *(p++);
            msg[i] = '\0';
        }
    }
    return 0;    
}

/*----------------------------------------------------
Read message                                        */
int read_message(int smsIndex, char *msg,int length)
{
    int i = 0;
    char gprsBuffer[100];
    char cmd[16];
    char *p,*s;
    
    sim900_check_with_cmd("AT+CMGF=1\r\n","OK",DEFAULT_TIMEOUT,CMD);
    suli_delay_ms(1000);
    sprintf(cmd,"AT+CMGR=%d\r\n",smsIndex);
    sim900_send_cmd(cmd);
    sim900_clean_buffer(gprsBuffer,sizeof(gprsBuffer));
    sim900_read_buffer(gprsBuffer,sizeof(gprsBuffer),DEFAULT_TIMEOUT);

    if(NULL != ( s = strstr(gprsBuffer,"+CMGR:")))
    {
        if(NULL != ( s = strstr(s,"\r\n")))
        {
            p = s + 2;
            while((*p != '\r')&&(i < length-1)) msg[i++] = *(p++);
            msg[i] = '\0';
        }
    }
    return 0;   
}

/*----------------------------------------------------
Delete message                                      */
int delete_message(int index)
{
    char cmd[16];
    snprintf(cmd,sizeof(cmd),"AT+CMGD=%d\r\n",index);
    if(0 != sim900_check_with_cmd(cmd,"OK",DEFAULT_TIMEOUT,CMD)) return -1;
    return 0;    
}

/*----------------------------------------------------
Make call                                           */
int make_call(char *num)
{
    char cmd[24];
    if(0 != sim900_check_with_cmd("AT+COLP=1\r\n","OK",DEFAULT_TIMEOUT,CMD)) return -1;
    suli_delay_ms(1000);
    sprintf(cmd,"ATD%s;\r\n", num);
    sim900_send_cmd(cmd);
    return 0;
}

/*----------------------------------------------------
Answer to call                                      */
int answer(void)
{
    sim900_send_cmd("ATA\r\n");
    return 0;
}

/*----------------------------------------------------
check if GPRS module is readable or not             */
int GPRS::readable(void)
{
    return sim900_check_readable();
}

/*----------------------------------------------------
check few times if GPRS module is readable or not   */
int GPRS::wait_readable(int wait_time)
{
    return sim900_wait_readable(wait_time);
}

