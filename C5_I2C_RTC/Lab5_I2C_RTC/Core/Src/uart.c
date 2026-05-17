///*
// * uart.c
// *
// *  Created on: Sep 26, 2023
// *      Author: HaHuyen
// */
//#include "uart.h"
//#include "fsm_clock.h"
//
//uint8_t receive_buffer1 = 0; //nhận 1 byte
//
//uint8_t msg[100];
//
//#define UART_BUFFER_SIZE 64
//volatile uint8_t uart_rx_buffer[UART_BUFFER_SIZE];
//
//volatile uint16_t uart_rx_head = 0;  	//receive
//volatile uint16_t uart_rx_tail = 0;		//receive
//
//
////--------C5-bai2-------//
//// Buffer để lưu lệnh từ PC
//char uart_cmd_buffer[32];
//uint8_t uart_cmd_index = 0;
//volatile uint8_t uart_cmd_ready = 0; // Cờ báo có lệnh mới từ PC
//
//// Biến tạm để lưu thời gian trước khi ghi vào DS3231
////extern uint8_t temp_hour, temp_min, temp_sec, temp_date, temp_month, temp_year;
////--------C5-bai2-------//
//
//
//
//
//
//////----------------------c5-b1----------------------//
////void process_uart_data(void) {
////    // Kiểm tra xem có dữ liệu mới trong buffer không
////    if (uart_rx_tail != uart_rx_head) {
////        // Lấy 1 byte từ buffer
////        uint8_t data = uart_rx_buffer[uart_rx_tail];
////
////        // Cập nhật vị trí 'tail'
////        uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;
////
////        // --- Xử lý dữ liệu 'data' ở đây ---
////        // Ví dụ: Gửi (echo) lại máy tính để kiểm tra
////        HAL_UART_Transmit(&huart1, &data, 1, 10);
////    }
////}
//////----------------------c5-b1----------------------//
//
////--c5-b2--//
//void process_uart_data(void) {
//    if (uart_cmd_ready) return; // Đợi lệnh cũ được xử lý
//
//    while (uart_rx_tail != uart_rx_head) {
//        uint8_t data = uart_rx_buffer[uart_rx_tail];
//
//        uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;
//
//        if (data == '\r' || data == '\n') { // Khi nhấn Enter trên Hercules
//            if (uart_cmd_index > 0) { // Nếu có dữ liệu
//                uart_cmd_buffer[uart_cmd_index] = '\0'; // Kết thúc chuỗi
//                uart_cmd_index = 0; // Reset index
//                uart_cmd_ready = 1; // Bật cờ báo có lệnh
//            }
//        } else if (uart_cmd_index < sizeof(uart_cmd_buffer) - 1) {
//            uart_cmd_buffer[uart_cmd_index] = data;
//            uart_cmd_index++;
//        }
//    }
//}
//
//void uart_init_rs232(){
//	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
//}
//
//void uart_Rs232SendString(const char* str){
//	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
//}
//
//void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
//	HAL_UART_Transmit(&huart1, bytes, size, 10);
//}
//
//void uart_Rs232SendNum(uint32_t num){
//	if(num == 0){
//		uart_Rs232SendString("0");
//		return;
//	}
//    uint8_t num_flag = 0;
//    int i;
//	if(num < 0) uart_Rs232SendString("-");
//    for(i = 10; i > 0; i--)
//    {
//        if((num / mypow(10, i-1)) != 0)
//        {
//            num_flag = 1;
//            sprintf((void*)msg,"%d",num/mypow(10, i-1));
//            uart_Rs232SendString(msg);
//        }
//        else
//        {
//            if(num_flag != 0)
//            	uart_Rs232SendString("0");
//        }
//        num %= mypow(10, i-1);
//    }
//}
//
//void uart_Rs232SendNumPercent(uint32_t num)
//{
//	sprintf((void*)msg,"%ld",num/100);
//    uart_Rs232SendString(msg);
//    uart_Rs232SendString(".");
//    sprintf((void*)msg,"%ld",num%100);
//    uart_Rs232SendString(msg);
//}
//
////--------------------------- origin HAL_UART_RxCpltCallback ---------------------------//
////void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
////	if(huart->Instance == USART1){
////		// rs232 isr
////		// can be modified
////		HAL_UART_Transmit(&huart1, &receive_buffer1, 1, 10);
////
////
////		// turn on the receice interrupt
////		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
////	}
////}
////--------------------------- origin ---------------------------//
//
//// B1: Nhận 1 byte
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
//	if(huart->Instance == USART1){
//
//		uint16_t next_head = (uart_rx_head + 1) % UART_BUFFER_SIZE;
//
//		if (next_head != uart_rx_tail) {
//		            // Buffer chưa đầy, lưu byte nhận được
//		            uart_rx_buffer[uart_rx_head] = receive_buffer1;
//		            uart_rx_head = next_head; // Cập nhật head
//		        }
//
//		// turn on the receice interrupt - Kích hoạt lại ngắt để nhận byte tiếp theo,
//		// kể cả khi buffer đầy (để tránh mất byte đầu tiên khi tail được đọc)
//		HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
//	}
//}





/*
 * uart.c
 *
 * Created on: Sep 26, 2023
 * Author: HaHuyen
 */
#include "uart.h"
#include "fsm_clock.h"
#include "lcd.h"

uint8_t receive_buffer1 = 0; //nhận 1 byte

uint8_t msg[100];

#define UART_BUFFER_SIZE 64
volatile uint8_t uart_rx_buffer[UART_BUFFER_SIZE];

volatile uint16_t uart_rx_head = 0;  	//receive
volatile uint16_t uart_rx_tail = 0;		//receive


//--------C5-bai2-------//
// Buffer để lưu lệnh từ PC
char uart_cmd_buffer[32];
uint8_t uart_cmd_index = 0;
volatile uint8_t uart_cmd_ready = 0; // Cờ báo có lệnh mới từ PC

// Biến tạm để lưu thời gian trước khi ghi vào DS3231
//extern uint8_t temp_hour, temp_min, temp_sec, temp_date, temp_month, temp_year;
//--------C5-bai2-------//





////----------------------c5-b1----------------------//
//void process_uart_data(void) {
//    // Kiểm tra xem có dữ liệu mới trong buffer không
//    if (uart_rx_tail != uart_rx_head) {
//        // Lấy 1 byte từ buffer
//        uint8_t data = uart_rx_buffer[uart_rx_tail];
//
//        // Cập nhật vị trí 'tail'
//        uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;
//
//        // --- Xử lý dữ liệu 'data' ở đây ---
//        // Ví dụ: Gửi (echo) lại máy tính để kiểm tra
//        HAL_UART_Transmit(&huart1, &data, 1, 10);
//    }
//}
////----------------------c5-b1----------------------//

//--c5-b2--//
void process_uart_data(void) {
    if (uart_cmd_ready) return; // Đợi lệnh cũ được xử lý

    while (uart_rx_tail != uart_rx_head) {
        uint8_t data = uart_rx_buffer[uart_rx_tail];

        uart_rx_tail = (uart_rx_tail + 1) % UART_BUFFER_SIZE;

        if (data == '\r' || data == '\n') { // Khi nhấn Enter trên Hercules
            if (uart_cmd_index > 0) { // Nếu có dữ liệu
                uart_cmd_buffer[uart_cmd_index] = '\0'; // Kết thúc chuỗi
                uart_cmd_index = 0; // Reset index
                uart_cmd_ready = 1; // Bật cờ báo có lệnh
            }
        } else if (uart_cmd_index < sizeof(uart_cmd_buffer) - 1) {
            uart_cmd_buffer[uart_cmd_index] = data;
            uart_cmd_index++;
        }
    }
}

void uart_init_rs232(){
	HAL_UART_Receive_IT(&huart1, &receive_buffer1, 1);
}

//void uart_Rs232SendString(const char* str){
//	HAL_UART_Transmit(&huart1, (void*)msg, sprintf((void*)msg,"%s",str), 10);
//}

// THÊM HÀM MỚI NÀY:
void uart_Rs232SendString(const char* str) {
    int len = strlen(str); // Lấy độ dài chuỗi
    HAL_UART_Transmit(&huart1, (uint8_t*)str, len, 100); // Gửi trực tiếp
}

void uart_Rs232SendBytes(uint8_t* bytes, uint16_t size){
	HAL_UART_Transmit(&huart1, bytes, size, 10);
}

//void uart_Rs232SendNum(uint32_t num){
//	if(num == 0){
//		uart_Rs232SendString("0");
//		return;
//	}
//    uint8_t num_flag = 0;
//    int i;
//	if(num < 0) uart_Rs232SendString("-");
//    for(i = 10; i > 0; i--)
//    {
//        if((num / mypow(10, i-1)) != 0)
//        {
//            num_flag = 1;
//            sprintf((void*)msg,"%d",num/mypow(10, i-1));
//            uart_Rs232SendString(msg);
//        }
//        else
//        {
//            if(num_flag != 0)
//            	uart_Rs232SendString("0");
//        }
//        num %= mypow(10, i-1);
//    }
//}
//
//void uart_Rs232SendNumPercent(uint32_t num)
//{
//	sprintf((void*)msg,"%ld",num/100);
//    uart_Rs232SendString(msg);
//    uart_Rs232SendString(".");
//    sprintf((void*)msg,"%ld",num%100);
//    uart_Rs232SendString(msg);
//}

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
