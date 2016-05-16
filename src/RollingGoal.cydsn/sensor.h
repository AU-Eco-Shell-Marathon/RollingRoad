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

struct sample
{
    float avg;
    float rms;
    float min;
    float max;
};

struct data
{
    struct sample V_motor;
    struct sample A_motor;
    struct sample P_motor;
    struct sample RPM;
    struct sample Moment;
    struct sample P_mekanisk;
    struct sample efficiency;
    uint32 distance;
    uint16 Alpha;
    float maxRMS;
    uint32 time_ms;
    char stop;
};


char getData(struct data *);
float getMoment();
int32 getDistance(char reset);

void setSNRdB(float SNRdB);

void sensor_init(int32 VM, int32 AM, int32 moment);
void sensor_calibrate(int32* VM, int32* AM, int32* moment);
float MomentToForce(float Moment_value);
float ForceToMoment(float Force_value);
float RPMToSpeed(float RPM_value);


#endif

/* [] END OF FILE */

