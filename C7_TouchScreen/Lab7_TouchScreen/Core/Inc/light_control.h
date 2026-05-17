/*
 * light_control.h
 *
 *  Created on: Nov 20, 2025
 *      Author: vinhd
 */

#include "gpio.h"
#include "uart.h"
#include "button.h"
#include "lcd.h"

extern uint8_t light_status;

void lightProcess();
void test_Esp();
