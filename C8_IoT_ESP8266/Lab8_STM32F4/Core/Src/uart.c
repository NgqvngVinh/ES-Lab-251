/*
 * uart.c
 *
 *  Created on: Sep 26, 2023
 *      Author: HaHuyen
 */
#include "uart.h"

// Declare UART1 (RS232)
uint8_t receive_buffer1 = 0; //nhận 1 byte
uint8_t msg[100];

extern uint8_t light_status;

// Declare UART2 (ESP)
uint8_t receive_buffer2 = 0;
uint8_t check_esp = 0;



void uart_Rs232SendNum(uint32_t num){
	if(num == 0){
		uart_Rs232SendString((uint8_t*)"0");
		return;
	}
    uint8_t num_flag = 0;

	if(num < 0)
		uart_Rs232SendString((uint8_t*)"-");

    int i;
	for(i = 10; i > 0; i--)
    {
        if((num / mypow(10, i-1)) != 0)
        {
            num_flag = 1;
            sprintf((char*)msg,"%ld",num/mypow(10, i-1));
            uart_Rs232SendString(msg);
        }
        else
        {
            if(num_flag != 0)
            	uart_Rs232SendString((uint8_t*)"0");
        }
        num %= mypow(10, i-1);
    }
}

void uart_Rs232SendNumPercent(uint32_t num)
{
	sprintf((char*)msg,"%ld",num/100);
    uart_Rs232SendString(msg);
    uart_Rs232SendString((uint8_t*)".");
    sprintf((char*)msg,"%ld",num%100);
    uart_Rs232SendString(msg);
}

void uart_init_esp(){
	HAL_UART_Receive_IT(&huart2, &receive_buffer2, 1);
}

// Gửi dữ liệu sang ESP
void uart_EspSendBytes(uint8_t* bytes, uint16_t size){
    HAL_UART_Transmit(&huart2, bytes, size, 10);
}

// Kiểm tra trạng thái ESP
uint8_t uart_EspCheck(){
    return check_esp;
}


// --- HÀM NGẮT (ISR) ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

	if( huart->Instance == USART2 ) {
	// esp isr
	if( receive_buffer2 == 'O' || receive_buffer2 == 'o')
		check_esp = 1;
	else if( receive_buffer2 == 'a')
		light_status = 0;
	else if( receive_buffer2 == 'A')
		light_status = 1;

	// enable receive
	HAL_UART_Receive_IT (& huart2 , &receive_buffer2 , 1) ;
	}

}
