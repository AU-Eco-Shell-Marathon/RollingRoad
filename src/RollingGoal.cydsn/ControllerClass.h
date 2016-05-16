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
#ifndef CONTROLLERCLASS_H
#define CONTROLLERCLASS_H
// Heap size min = 0x0200 !!!


#include "PID.h"
#include "sensor.h"
#include "Uart.h"
#include "EEPROM.h"

void run();
void stop();
void update(const struct PIDparameter *, const float * Force, char restart);
void init();
void calibrate();

#endif
/* [] END OF FILE */
