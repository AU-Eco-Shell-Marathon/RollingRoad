#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>
#include "sensor.h"
    
//Functions
void UART_init();
char UART_checkConnection(void);
void UART_receiveUARTData();
void UART_send(char *Pdata);
void UART_sendData(struct data* Data,  float set_torque, float *PIDdebug);

#endif