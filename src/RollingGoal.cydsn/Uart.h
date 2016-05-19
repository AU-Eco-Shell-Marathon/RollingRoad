#ifndef UART_H
#define UART_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <project.h>
#include "sensor.h"
    
//Functions
void InitUart();
char CheckConnection(void);
void ReceiveUARTData();
void SendUART(char *Pdata);
void SendData(struct data* Data,  float set_torque, float *PIDdebug);

#endif