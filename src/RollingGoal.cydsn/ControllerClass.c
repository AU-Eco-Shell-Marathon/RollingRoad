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
/*
#define START_EEPROM_SECTOR  (1u) // vil tro at man godt kan bruge helt fra SECTOR 0, hvis man skal bruge mere plads.
#define PID_BYTES         ((START_EEPROM_SECTOR * EEPROM_SIZEOF_SECTOR) + 0x00)
*/

//float set_force=21.76f;
float set_force=5.5f;

/*
char save(const struct PIDparameter * PID,const float * moment);
char load(struct PIDparameter * PID, float * moment);
*/
//struct sekvens * seq = NULL;


CY_ISR_PROTO(PID_isr);


float *PIDdebug;

float Moment_debug;


CY_ISR(PID_isr)
{  
    Moment_debug = getMoment();
    PIDdebug = PID_tick(Moment_debug, ForceToMoment(set_force));
}

char busy = 0;
CY_ISR(SendData_ISR)
{

    busy = 0;
}

/* 
struct sample
{
    int32 avg;
    float rms;
    int32 min;
    int32 max;
};

struct data
{
    struct sample V_motor;
    struct sample A_motor;
    struct sample P_motor;
    struct sample RPM;
    struct sample Moment;
    struct sample P_mekanisk;
    uint32 distance;
    uint32 time_ms;
    char stop;
};
*/

void run()
{
    
    ReceiveUARTData(); 
    
    struct data Data;
    if(getData(&Data) && !busy)
    {
        SendData(&Data, set_force, ForceToMoment(set_force), Moment_debug, PIDdebug);
        busy = 1;
    }
}

void stop()
{
    
    
}

void calibrate()
{
    
    int32 Offset[4];

    sensor_calibrate(&Offset[0], &Offset[1], &Offset[2], &Offset[3]);
    
    EEPROM_write(2, (uint8*)Offset);
    
}


void update(const struct PIDparameter * parameter, const float * Force, char restart)
{
    if(parameter != NULL)
    {    
        setPID(parameter);
        EEPROM_write(0, (uint8*)parameter);
    }
    
    if(Force != NULL)
    {
        set_force=*Force;
        EEPROM_write(1, (uint8*)Force);
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
    
    size_t sizes[] = {sizeof(struct PIDparameter), sizeof(float), sizeof(int32[4])};
   
    EEPROM_init(sizes, 3);
    
    int32 Offset[4];
    
    if(EEPROM_read(2, (uint8*)Offset))
        sensor_init(Offset[0],Offset[1],Offset[2],Offset[3]);
    else
        sensor_init(0,0,0,0);
    
    //sensor_init(0,0,0,0);
    PID_init();
    
    EEPROM_read(0,(uint8*)getPID_ptr());
    EEPROM_read(1,(uint8*)&set_force);
    
    isr_4_StartEx(PID_isr);
    Clock_4_Start();
    
    Clock_5_Start();
    isr_5_StartEx(SendData_ISR);
    
    
    TX_AND_POWER_Write(1);
}

/*
char save(const struct PIDparameter * PID, const float * moment)
{
    EEPROM_1_WriteByte(0x00, PID_BYTES);
    uint16 i;
    uint8 size_PID = sizeof(struct PIDparameter);
    uint8 size_MOMENT = sizeof(float);
    uint size = size_PID + size_MOMENT + 1;
    
    if(size > EEPROM_SIZEOF_SECTOR)
    {
        return -2; //fylder for meget.
    }
    
    uint8 *data = (uint8*)PID;
    if(PID != NULL)
    {
        for(i = 1; i < size_PID + 1; i++)
        {
            if(EEPROM_1_WriteByte(data[i], PID_BYTES + i) != CYRET_SUCCESS)
            {
                return -1; //fejl i tilskrivning
            }
        }
    }
    
    data = (uint8*)moment;
    
    if(moment != NULL)
    {
        for(i = size_PID + 1; i < size; i++)
        {
            if(EEPROM_1_WriteByte(data[i], PID_BYTES + i) != CYRET_SUCCESS)
            {
                return -1; //fejl i tilskrivning
            }
        }
    }
    
    EEPROM_1_WriteByte(0x33, PID_BYTES);
    return 1;
}

char load(struct PIDparameter * PID, float * moment)
{
    uint16 i;
    uint8 size_PID = sizeof(struct PIDparameter);
    uint8 size_MOMENT = sizeof(float);
    uint size = size_PID + size_MOMENT + 1;
    
    if(size > EEPROM_SIZEOF_SECTOR)
    {
        return -2; //fylder for meget.
    }
    
    if(EEPROM_1_ReadByte(PID_BYTES) != 0x33)
    {
        return 0; // ikke noget gemt data
    }
    uint8 * data = (uint8*)PID;
    for(i = 1; i < size_PID + 1; i++)
    {
        data[i] = EEPROM_1_ReadByte(PID_BYTES + i);
    }
    data = (uint8*)moment;
    for(i = size_PID + 1; i < size; i++)
    {
        data[i] = EEPROM_1_ReadByte(PID_BYTES + i);
    }
    
    return 1;
}
*/

/* [] END OF FILE */
