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
    PIDdebug = PID_tick(getMoment(), set_torque, getRPM());
}

CY_ISR(SendData_ISR)
{

    busy = 0;
}

void run()
{
    
    ReceiveUARTData(); 
    
    struct data Data;
    if(getData(&Data) && !busy)
    {
        SendData(&Data, set_torque, PIDdebug);
        busy = 1;
    }
}

void stop()
{
    
    
}

void calibrate()
{
    
    int32 Offset[3];

    sensor_calibrate(&Offset[0], &Offset[1], &Offset[2]);
    
    EEPROM_write(2, (uint8*)Offset);
    
}


void update(const struct PIDparameter * parameter, const float * torque, char restart)
{
    if(parameter != NULL)
    {    
        setPID(parameter);
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
        getDistance(0b1);
    }
    
}

void init()
{
    TX_AND_POWER_Write(0);
    CyGlobalIntEnable; /* Enable global interrupts. */
    InitUart();
    
    size_t sizes[] = {sizeof(struct PIDparameter), sizeof(float), sizeof(int32[3])};
   
    EEPROM_init(sizes, 3);
    
    int32 Offset[3];
    
    if(EEPROM_read(2, (uint8*)Offset))
        sensor_init(Offset[0],Offset[1],Offset[2]);
    else
        sensor_init(0,0,0);
    
    PID_init();
    
    EEPROM_read(0,(uint8*)getPID_ptr());
    EEPROM_read(1,(uint8*)&set_torque);
    
    isr_4_StartEx(PID_isr);
    Clock_4_Start();
    
    Clock_5_Start();
    isr_5_StartEx(SendData_ISR);
    
    
    TX_AND_POWER_Write(1);
}

/* [] END OF FILE */
