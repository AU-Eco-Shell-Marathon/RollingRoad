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
#include "Sensor.h"
#include "UART.h"
#include "EEPROM.h"

void ControllerClass_run();
void ControllerClass_stop();
void ControllerClass_update(const struct PIDparameter *, const float * torque, char restart);
void ControllerClass_init();
void ControllerClass_calibrate();

#endif
/* [] END OF FILE */
