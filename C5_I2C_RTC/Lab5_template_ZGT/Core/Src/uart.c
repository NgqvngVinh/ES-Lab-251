/*
 * uart.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "uart.h"

uint8_t receive_buffer1 = 0; //nhận 1 byte

uint8_t msg[100];

void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

void uart_Rs232SendString(uint8_t* str){
	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString("0");
		return;
	}
    uint8_t num_flag = 0;
    int i;
	if(num < 0) uart_Rs232SendString("-");
    for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((void*)msg,"%d",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString("0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((void*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString(".");
    sprintf((void*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}

//--------------------------- origin HAL_UART_RxCpltCallback ---------------------------//
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart->Instance == USART1){
//		// rs232 isr
//		// can be modified
//		HAL_UART_Transmit(&huart1, &receive_buffer1, 1, 10);
//
//
//		// turn on the receice interrupt
//		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
//	}
//}
//--------------------------- origin ---------------------------//

// B1: Nhận 1 byte
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
	if(huart->Instance == USART1){

		uint16_t next_head = (uart_rx_head + 1) % UART_BUFFER_SIZE;

		if (next_head != uart_rx_tail) {
		            // Buffer chưa đầy, lưu byte nhận được
		            uart_rx_buffer[uart_rx_head] = receive_buffer1;
		            uart_rx_head = next_head; // Cập nhật head
		        }

		// turn on the receice interrupt - Kích hoạt lại ngắt để nhận byte tiếp theo,
		// kể cả khi buffer đầy (để tránh mất byte đầu tiên khi tail được đọc)
		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
	}
}
