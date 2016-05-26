/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "ControllerClass.h"
#include <project.h>
#include <stdio.h>
#include "Constants.h"

CY_ISR_PROTO(PID_isr);

float set_torque=0.0f;
char busy = 0;

//debug 
float *PIDdebug;

CY_ISR(PID_isr)
{  
    PIDdebug = PID_tick(Sensor_getMoment(), set_torque, Sensor_getRPM());
}

CY_ISR(UART_sendData_ISR)
{

    busy = 0;
}

void ControllerClass_run()
{
    
    UART_receiveUARTData(); 
    
    struct data Data;
    if(Sensor_getData(&Data) && !busy)
    {
        UART_sendData(&Data, set_torque, PIDdebug);
        busy = 1;
    }
}

void ControllerClass_stop()
{
    
    
}

void ControllerClass_calibrate()
{
    
    int32 Offset[3];

    Sensor_calibrate(&Offset[0], &Offset[1], &Offset[2]);
    
    EEPROM_write(2, (uint8*)Offset);
    
}


void ControllerClass_update(const struct PIDparameter * parameter, const float * torque, char restart)
{
    if(parameter != NULL)
    {    
        PID_set(parameter);
        EEPROM_write(0, (uint8*)parameter);
    }
    
    if(torque != NULL)
    {
        set_torque=*torque;
        EEPROM_write(1, (uint8*)torque);
    }
    
    
    if(restart)
    {
        Control_Reg_1_Write(0b1);
        Sensor_getDistance(0b1);
    }
    
}

void ControllerClass_init()
{
    TX_AND_POWER_Write(0);
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_init();
    
    size_t sizes[] = {sizeof(struct PIDparameter), sizeof(float), sizeof(int32[3])};
   
    EEPROM_init(sizes, 3);
    
    int32 Offset[3];
    
    if(EEPROM_read(2, (uint8*)Offset))
        Sensor_init(Offset[0],Offset[1],Offset[2]);
    else
        Sensor_init(0,0,0);
    
    PID_init();
    
    EEPROM_read(0,(uint8*)PID_getPtr());
    EEPROM_read(1,(uint8*)&set_torque);
    
    isr_4_StartEx(PID_isr);
    Clock_4_Start();
    
    Clock_5_Start();
    isr_5_StartEx(UART_sendData_ISR);
    
    
    TX_AND_POWER_Write(1);
}

/* [] END OF FILE */
