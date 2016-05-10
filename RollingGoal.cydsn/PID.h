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

#define Kp_def 0.0;
#define Ki_def 0.1f;
#define Kd_def 0;
#define MAX_def 255;
#define MIN_def 0;
#define iMAX_def 100;
#define iMIN_def -100;
#define dt_def 0.0042f;

struct PIDparameter
{
    float Kp;
    float Ki;
    float Kd;
    float MAX;
    float MIN;
    float iMAX;
    float iMIN;
    char valid;
};

void PID_init();
float* PID_tick(float sensor, float input);
void setPID(const struct PIDparameter * parameter);
struct PIDparameter *getPID_ptr();
#endif
/* [] END OF FILE */
