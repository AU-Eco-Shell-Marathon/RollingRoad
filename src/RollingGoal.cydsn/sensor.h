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

#ifndef SENSOR_H
#define SENSOR_H
    
#include <project.h>


    
struct data
{
    float V_motor;
    float A_motor;
    float P_motor;
    float RPM;
    float Moment;
    float P_mekanisk;
    float efficiency;
    uint32 distance;
    uint16 Alpha;
    uint32 time_ms;
    char stop;
};    
    
void Sensor_init(int32 VM, int32 AM, int32 moment);
void Sensor_calibrate(int32* VM, int32* AM, int32* moment);
char Sensor_getData(struct data *);
float Sensor_getMoment();
float Sensor_getRPM();
int32 Sensor_getDistance(char reset);

#endif

/* [] END OF FILE */

