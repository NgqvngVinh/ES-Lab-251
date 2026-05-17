/*
 * fsm_clock.c
 *
 *  Created on: Nov 5, 2025
 *      Author: PC
 */

#include "fsm_clock.h"
#include "ds3231.h"
#include "lcd.h"
#include "button.h"
#include "main.h"
#include "uart.h"
#include "utils.h"

// Định nghĩa các biến global
ClockMode clock_mode;
AdjustParam adjust_param;
AlarmParam alarm_param;

uint8_t temp_hour, temp_min, temp_sec, temp_day, temp_date, temp_month, temp_year;
uint8_t alarm_hour = 0, alarm_min = 0;

uint8_t is_alarming = 0;
uint8_t blink_ON = 0;
int blink_counter = 0;
volatile uint16_t uart_timeout_counter = 0;
uint8_t uart_retry_counter = 0;

uart_state uart_set_state = UART_SET_IDLE;

uint8_t request_sent = 0;
// uart_cmd_buffer and uart_cmd_ready are defined in uart.c (shared resource)
// extern declarations are in fsm_clock.h





void fsm_clock_init() {
	clock_mode = MODE_VIEW_TIME;
	adjust_param = ADJUST_HOUR;
	alarm_param = ALARM_HOUR;
	is_alarming = 0;
}

void fsm_clock_run() {
	if(clock_mode == MODE_VIEW_TIME || is_alarming) {
		 ds3231_ReadTime();
	}

	// 2Hz = 500ms/lan -> 10 ticks (1 tick = 50ms)
	// blink_counter < 5 -> lcd k hien thi j het
	// blink_counter >=5 -> lcd hien thi
	blink_counter = (blink_counter + 1) % 10;
	blink_ON = (blink_counter < 5);

	// Check va handle cai nut doi mode co dang bam k
	handle_mode_switch();

	switch (clock_mode) {
	case MODE_VIEW_TIME: 			// Mode nay k can handle nut bam j het nen empty
		break;

	case MODE_ADJUST_TIME:
		handle_adjust_time_btn();
		break;

	case MODE_SET_ALARM:
		handle_set_alarm_btn();
		break;

	case MODE_UART_SET:
		handle_uart_set_mode();
		break;
	}


	// Handle het cai phan display
	display_clock();
}

void handle_mode_switch() {
	if (button_count[BTN_MODE] == 1) {
		clock_mode = (clock_mode + 1) % 4; //add them mode uart c5
		lcd_Clear(BLACK);
	}
	if (clock_mode == MODE_UART_SET) {
		uart_set_state = UART_SET_IDLE; // Reset trạng thái
		request_sent = 0;
	}
}

void handle_adjust_time_btn(){
	// 40 * 50ms = 2s and 4 ticks (200ms) trig 1 lan
	if (button_count[BTN_INCREMENT] == 1 || (button_count[BTN_INCREMENT] >= 40 && (button_count[BTN_INCREMENT] - 40) % 4 == 0)) {
		increment();
	}
	// Bam nut confirm thi nhay qua adjust param tiep theo (vd: HOUR -> MIN -> SEC... -> YEAR -> HOUR)
	// Moi lan bam nut thi se luu lai thoi gian (save_time())
	if (button_count[BTN_CONFIRM] == 1) {
		save_time();
		if (adjust_param == ADJUST_YEAR) {
			adjust_param = ADJUST_HOUR;
		}
		else {
			adjust_param++;
		}
	}
}

void handle_set_alarm_btn() {
	// 40 * 50ms = 2s and 4 ticks (200ms) trig 1 lan
	if (button_count[BTN_INCREMENT] == 1 || (button_count[BTN_INCREMENT] >= 40 && (button_count[BTN_INCREMENT] - 40) % 4 == 0)) {
		if (alarm_param == ALARM_HOUR) {
			alarm_hour = (alarm_hour + 1) % 24;
		} else if (alarm_param == ALARM_MINUTE) {
			alarm_min = (alarm_min + 1) % 60;
		}
	}

	if (button_count[BTN_CONFIRM] == 1) {
		if (alarm_param == ALARM_MINUTE) {
			alarm_param = ALARM_HOUR;
		}
		else {
			alarm_param++;
		}
	}
}

// Hàm này được gọi trong while(1) khi clock_mode == MODE_UART_SET
void handle_uart_set_mode(void) {
//--------C5-bai3-------//
	// 0. KIỂM TRA TIMEOUT
	if (uart_timeout_counter >= UART_TIMEOUT_TICKS) {
		uart_timeout_counter = 0; // Reset bộ đếm
		uart_retry_counter++;     // Tăng số lần thử
		request_sent = 0;         // Đặt cờ để gửi lại request

		if (uart_retry_counter >= UART_MAX_RETRIES) {
			// Thử quá 3 lần, báo lỗi và thoát
			lcd_Clear(BLACK);
			lcd_ShowStr(10, 50, (uint8_t *)"Error: Timeout", RED, BLACK, 16, 0);
			HAL_Delay(2000);

			// Reset về trạng thái ban đầu

			uart_set_state = UART_SET_IDLE;

			clock_mode = MODE_VIEW_TIME;
			uart_retry_counter = 0;
			request_sent = 0;
			lcd_Clear(BLACK);
			return;
		}
	}
//--------C5-bai3-------//

    // 1. Xử lý khi nhận được lệnh (phản hồi) từ PC - KIỂM TRA PHẢN HỒI TỪ PC (lệnh đã sẵn sàng)
    if(uart_cmd_ready) {
        uart_cmd_ready = 0;
        //--------C5-bai3-------//
        uart_timeout_counter = 0;   // Reset timeout
		uart_retry_counter = 0;     // Reset số lần thử

		request_sent = 0;           // Chuẩn bị cho request tiếp theo
        //--------C5-bai3-------//
        int value = atoi(uart_cmd_buffer); // Chuyển chuỗi thành số
        uint8_t valid_data = 0;

        // 2. Gửi yêu cầu (request) đến PC
        switch (uart_set_state) {
            case UART_SET_WAIT_HOUR:
            	if(value >= 0 && value <= 23){
                    temp_hour = (uint8_t)value;
                    uart_set_state = UART_SET_WAIT_MIN; // Chuyển sang trạng thái kế
                    valid_data = 1;
            	}
                break;

            case UART_SET_WAIT_MIN:
            	if(value >= 0 && value <= 59){
                    temp_min = (uint8_t)value;
                    uart_set_state = UART_SET_SEC;
                    valid_data = 1;
            	}
                break;

            case UART_SET_SEC:
            	if (value >= 0 && value <= 59) {
                    temp_sec = (uint8_t)value;
                    uart_set_state = UART_SET_DATE;
                    valid_data = 1;
            	}
                break;

            case UART_SET_DATE:
            	if (value >= 1 && value <= 31) {
                    temp_date = (uint8_t)value;
                    uart_set_state = UART_SET_MONTH;
                    valid_data = 1;
            	}
                break;

            case UART_SET_MONTH:
            	if (value >= 1 && value <= 12) {
                    temp_month = (uint8_t)value;
                    uart_set_state = UART_SET_YEAR;
                    valid_data = 1;
            	}
                break;

            case UART_SET_YEAR:
            	if (value >= 0 && value <= 99) {
                    temp_year = (uint8_t)value;
                    uart_set_state = UART_SET_SAVING; // Đã nhận đủ, chuyển sang lưu
                    valid_data = 1;
            	}
                break;

            default:
                break;
        }
        // Kiểm tra dữ liệu không hợp lệ
			if (!valid_data) {
				// Dữ liệu không hợp lệ (ví dụ: giờ = 25, phút = 99)
				// Cứ để im, request_sent = 0 sẽ khiến request được gửi lại
				lcd_ShowStr(10, 70, (uint8_t *)"Invalid Data!", RED, BLACK, 16, 0);
				HAL_Delay(1000); // Hiển thị lỗi 1s
				lcd_ShowStr(10, 70, (uint8_t *)"             ", RED, BLACK, 16, 0); // Xóa lỗi
			}
    }





    // 2. Gửi yêu cầu (request) đến PC
    // (nếu chưa gửi)
    if (!request_sent) {
		uart_timeout_counter = 0; // Bắt đầu đếm timeout từ đây
		request_sent = 1;         // Đánh dấu đã gửi

    switch (uart_set_state) {
        case UART_SET_IDLE: // Trạng thái khi mới vào mode
        	request_sent = 0; // Chưa làm gì
            lcd_Clear(BLACK);
            uart_set_state = UART_SET_WAIT_HOUR; // Bắt đầu hỏi giờ
            break;

        case UART_SET_WAIT_HOUR:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating hours...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Hours\n");
            request_sent = 1;
            break;

        case UART_SET_WAIT_MIN:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating minutes...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Minutes\n");
            request_sent = 1;
            break;

        case UART_SET_SEC:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating seconds...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Seconds\n");
            request_sent = 1;
            break;

        case UART_SET_DATE:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating dates...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Dates\n");
            request_sent = 1;
            break;

        case UART_SET_MONTH:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating month...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Months\n");
            request_sent = 1;
            break;

        case UART_SET_YEAR:
            lcd_ShowStr(10, 50, (uint8_t *)"Updating year...", WHITE, BLACK, 16, 0);
            uart_Rs232SendString("Years\n");
            request_sent = 1;
            break;

        case UART_SET_SAVING:
        	request_sent = 0;
            lcd_ShowStr(10, 50, (uint8_t *)"Saving...", WHITE, BLACK, 16, 0);

            // Dùng hàm từ Lab 4 [cite: 1470, 1578] và thư viện utils.h [cite: 1440]
//            ds3231_Write(ADDRESS_HOUR, DEC2BCD(temp_hour));
//            ds3231_Write(ADDRESS_MIN, DEC2BCD(temp_min));
//            ds3231_Write(ADDRESS_SEC, DEC2BCD(temp_sec));
//            ds3231_Write(ADDRESS_DATE, DEC2BCD(temp_date));
//            ds3231_Write(ADDRESS_MONTH, DEC2BCD(temp_month));
//            ds3231_Write(ADDRESS_YEAR, DEC2BCD(temp_year));
            ds3231_Write(ADDRESS_HOUR, temp_hour);
            ds3231_Write(ADDRESS_MIN, temp_min);
            ds3231_Write(ADDRESS_SEC, temp_sec);
            ds3231_Write(ADDRESS_DATE, temp_date);
            ds3231_Write(ADDRESS_MONTH, temp_month);
            ds3231_Write(ADDRESS_YEAR, temp_year);

            lcd_ShowStr(10, 70, (uint8_t *)"Done!", WHITE, BLACK, 16, 0);
            HAL_Delay(1000);

            // Xong, quay về trạng thái cũ
            uart_set_state = UART_SET_IDLE;
            clock_mode = MODE_VIEW_TIME;
            lcd_Clear(BLACK);
            break;
        default:
            break;
    }
    }

    }
//--------------------c5-b2-------------------//





void increment() {
	switch(adjust_param) {
		case ADJUST_HOUR:   temp_hour = (temp_hour + 1) % 24; break;
		case ADJUST_MINUTE: temp_min = (temp_min + 1) % 60; break;
		case ADJUST_SECOND: temp_sec = (temp_sec + 1) % 60; break;
		case ADJUST_DAY:    temp_day = (temp_day % 7) + 1; break; // 1-7
		case ADJUST_DATE:   temp_date = (temp_date % 31) + 1; break; // 1-31
		case ADJUST_MONTH:  temp_month = (temp_month % 12) + 1; break; // 1-12
		case ADJUST_YEAR:   temp_year = (temp_year + 1) % 100; break; // 0-99
	}
}

//void save_time() {
//    // THÊM DEC2BCD() VÀO CÁC HÀM GHI
//	ds3231_Write(ADDRESS_HOUR, DEC2BCD(temp_hour));
//	ds3231_Write(ADDRESS_MIN, DEC2BCD(temp_min));
//	ds3231_Write(ADDRESS_SEC, DEC2BCD(temp_sec));
//	ds3231_Write(ADDRESS_DAY, DEC2BCD(temp_day));
//	ds3231_Write(ADDRESS_DATE, DEC2BCD(temp_date));
//	ds3231_Write(ADDRESS_MONTH, DEC2BCD(temp_month));
//	ds3231_Write(ADDRESS_YEAR, DEC2BCD(temp_year));
//
//}

void save_time() {
    // BỎ DEC2BCD(), ghi trực tiếp giá trị biến
    ds3231_Write(ADDRESS_HOUR, temp_hour);
    ds3231_Write(ADDRESS_MIN, temp_min);
    ds3231_Write(ADDRESS_SEC, temp_sec);
    ds3231_Write(ADDRESS_DAY, temp_day);
    ds3231_Write(ADDRESS_DATE, temp_date);
    ds3231_Write(ADDRESS_MONTH, temp_month);
    ds3231_Write(ADDRESS_YEAR, temp_year);
}


// check xem cai hour/min/sec o tren con chip no co trung voi thoi gian alarm duoc luu hay k
// co thi kich flag
void check_alarm() {
	if (clock_mode != MODE_VIEW_TIME || is_alarming) {
		return;
	}

	if (ds3231_hours == alarm_hour &&
		ds3231_min == alarm_min &&
		ds3231_sec == 0)
	{
		is_alarming = 1;
	}
}

void display_clock(){
	if (is_alarming) {
		display_alarm();
		return;
	}

	// MODE_UART_SET tự xử lý display trong handle_uart_set_mode()
	// Không cần clear màn hình vì sẽ gây nhấp nháy
	if (clock_mode == MODE_UART_SET) {
		return;
	}


	switch (clock_mode) {
	case MODE_VIEW_TIME:
		lcd_ShowStr(10, 10, (uint8_t *)"MODE: VIEW TIME", WHITE, BLACK, 16, 0);
		display_view_time();
		break;

	case MODE_ADJUST_TIME:
		lcd_ShowStr(10, 10, (uint8_t *)"MODE: ADJUST TIME", YELLOW, BLACK, 16, 0);
		display_adjust_time();
		break;

	case MODE_SET_ALARM:
		lcd_ShowStr(10, 10, (uint8_t *)"MODE: SET ALARM", CYAN, BLACK, 16, 0);
		display_set_alarm();
		break;

	default:
		break;
	}
}

void display_view_time() {
	// HOUR:MIN:SEC
	lcd_ShowIntNum(50, 80, ds3231_hours, 2, GREEN, BLACK, 24);
	lcd_ShowStr(80, 80, (uint8_t *)":", GREEN, BLACK, 24, 0);
	lcd_ShowIntNum(95, 80, ds3231_min, 2, GREEN, BLACK, 24);
	lcd_ShowStr(125, 80, (uint8_t *)":", GREEN, BLACK, 24, 0);
	lcd_ShowIntNum(140, 80, ds3231_sec, 2, GREEN, BLACK, 24);

	// DAY, DATE/MONTH/YEAR
	lcd_ShowStr(50, 120, (uint8_t *)"Day:", WHITE, BLACK, 16, 0);
	lcd_ShowIntNum(90, 120, ds3231_day, 1, WHITE, BLACK, 16);

	lcd_ShowIntNum(50, 150, ds3231_date, 2, WHITE, BLACK, 16);
	lcd_ShowStr(75, 150, (uint8_t *)"/", WHITE, BLACK, 16, 0);
	lcd_ShowIntNum(90, 150, ds3231_month, 2, WHITE, BLACK, 16);
	lcd_ShowStr(115, 150, (uint8_t *)"/20", WHITE, BLACK, 16, 0);
	lcd_ShowIntNum(145, 150, ds3231_year, 2, WHITE, BLACK, 16);

	// ALARM
	// HOUR:MIN
	lcd_ShowStr(50, 200, (uint8_t *)"Alarm:", GRAY, BLACK, 16, 0);
	lcd_ShowIntNum(110, 200, alarm_hour, 2, GRAY, BLACK, 16);
	lcd_ShowStr(135, 200, (uint8_t *)":", GRAY, BLACK, 16, 0);
	lcd_ShowIntNum(150, 200, alarm_min, 2, GRAY, BLACK, 16);
}

void display_adjust_time() {
	// HOUR
	if (adjust_param != ADJUST_HOUR || blink_ON)
		lcd_ShowIntNum(50, 80, temp_hour, 2, YELLOW, BLACK, 24);
	else lcd_Fill(50, 80, 50+24, 80+24, BLACK); // Xóa đi
	lcd_ShowStr(80, 80, (uint8_t *)":", YELLOW, BLACK, 24, 0);

	// MIN
	if (adjust_param != ADJUST_MINUTE || blink_ON)
		lcd_ShowIntNum(95, 80, temp_min, 2, YELLOW, BLACK, 24);
	else lcd_Fill(95, 80, 95+24, 80+24, BLACK);
	lcd_ShowStr(125, 80, (uint8_t *)":", YELLOW, BLACK, 24, 0);

	// SEC
	if (adjust_param != ADJUST_SECOND || blink_ON)
		lcd_ShowIntNum(140, 80, temp_sec, 2, YELLOW, BLACK, 24);
	else lcd_Fill(140, 80, 140+24, 80+24, BLACK);

	// DAY
	lcd_ShowStr(50, 120, (uint8_t *)"Day:", WHITE, BLACK, 16, 0);
	if (adjust_param != ADJUST_DAY || blink_ON)
		lcd_ShowIntNum(90, 120, temp_day, 1, YELLOW, BLACK, 16);
	else lcd_Fill(90, 120, 90+12, 120+16, BLACK);

	// DATE
	if (adjust_param != ADJUST_DATE || blink_ON)
		lcd_ShowIntNum(50, 150, temp_date, 2, YELLOW, BLACK, 16);
	else lcd_Fill(50, 150, 50+24, 150+16, BLACK);
	lcd_ShowStr(75, 150, (uint8_t *)"/", WHITE, BLACK, 16, 0);

	// MONTH
	if (adjust_param != ADJUST_MONTH || blink_ON)
		lcd_ShowIntNum(90, 150, temp_month, 2, YELLOW, BLACK, 16);
	else lcd_Fill(90, 150, 90+24, 150+16, BLACK);
	lcd_ShowStr(115, 150, (uint8_t *)"/20", WHITE, BLACK, 16, 0);

	// YEAR
	if (adjust_param != ADJUST_YEAR || blink_ON)
		lcd_ShowIntNum(145, 150, temp_year, 2, YELLOW, BLACK, 16);
	else lcd_Fill(145, 150, 145+24, 150+16, BLACK);
}

void display_set_alarm() {
	lcd_ShowStr(50, 100, (uint8_t *)"Set Alarm:", CYAN, BLACK, 24, 0);

	// HOUR
	if (alarm_param != ALARM_HOUR || blink_ON)
		lcd_ShowIntNum(80, 140, alarm_hour, 2, CYAN, BLACK, 24);
	else lcd_Fill(80, 140, 80+24, 140+24, BLACK);

	lcd_ShowStr(110, 140, (uint8_t *)":", CYAN, BLACK, 24, 0);

	// MIN
	if (alarm_param != ALARM_MINUTE || blink_ON)
		lcd_ShowIntNum(125, 140, alarm_min, 2, CYAN, BLACK, 24);
	else lcd_Fill(125, 140, 125+24, 140+24, BLACK);
}

void display_alarm() {
	if (blink_ON) {
		lcd_Clear(RED);
		lcd_StrCenter(0, 100, (uint8_t *)"ALARM!!!", BLACK, RED, 32, 0);
	} else {
		lcd_Clear(BLACK);
		lcd_StrCenter(0, 100, (uint8_t *)"ALARM!!!", RED, BLACK, 32, 0);
	}
}
