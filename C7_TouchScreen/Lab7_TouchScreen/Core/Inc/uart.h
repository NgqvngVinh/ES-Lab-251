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

// Function Prototypes
void uart_init_rs232();
void uart_Rs232SendString(uint8_t* str);
void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size);
void uart_Rs232SendNum(uint32_t num);
void uart_Rs232SendNumPercent(uint32_t num);

// Khai báo biến để các file khác dùng được
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern uint8_t check_esp;


// Thêm các hàm cho ESP
void uart_init_esp();
void uart_EspSendBytes(uint8_t* bytes, uint16_t size);
uint8_t uart_EspCheck();

#endif /* INC_UART_H_ */
