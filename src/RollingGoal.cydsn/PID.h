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
#ifndef PID_H
    #define PID_H

struct PIDparameter
{
    float Kp;
    float Ki;
    float Kd;
    float MAX;
    float MIN;
    char valid;
};

void PID_init();
float* PID_tick(float sensor, float input, float RPM);
void PID_set(const struct PIDparameter * parameter);
struct PIDparameter *PID_getPtr();
#endif
/* [] END OF FILE */
