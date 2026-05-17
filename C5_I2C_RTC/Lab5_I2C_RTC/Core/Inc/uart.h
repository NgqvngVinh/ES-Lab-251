/*
 * uart.h
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#ifndef INC_UART_H_
#define INC_UART_H_

#include "usart.h"
#include <stdio.h>
#include "utils.h"
#include "stdint.h"
#include <string.h> // <-- THÊM DÒNG NÀY


void uart_init_rs232();

void uart_Rs232SendString(const char* str);

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size);

//void uart_Rs232SendNum(uint32_t num);
//
//void uart_Rs232SendNumPercent(uint32_t num);

void process_uart_data(void);

#endif /* INC_UART_H_ */

