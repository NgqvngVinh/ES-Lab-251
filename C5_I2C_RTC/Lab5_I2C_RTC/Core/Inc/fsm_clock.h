/*
 * fsm_clock.h
 *
 *  Created on: Nov 5, 2025
 *      Author: PC
 */
#include <stdint.h>

#ifndef INC_FSM_CLOCK_H_
#define INC_FSM_CLOCK_H_

#define BTN_MODE      	0
#define BTN_INCREMENT	1
#define BTN_CONFIRM  	2



extern uint8_t temp_hour, temp_min, temp_sec, temp_day, temp_date, temp_month, temp_year;
extern uint8_t alarm_hour, alarm_min;

extern uint8_t is_alarming;
extern uint8_t blink_ON;
extern int blink_counter;
extern volatile uint16_t uart_timeout_counter; // Bộ đếm 50ms

//--------C5-bai3-------//
#define UART_TIMEOUT_TICKS 200 // 200 * 50ms = 10s
#define UART_MAX_RETRIES 3     // Gửi lại 3 lần [cite: 2161]

extern uint8_t uart_retry_counter; // Bộ đếm số lần thử lại
//--------C5-bai3-------//


typedef enum {
    MODE_VIEW_TIME,     // [cite: 1629]
    MODE_ADJUST_TIME,   // [cite: 1631]
    MODE_SET_ALARM,      // [cite: 1636]
	MODE_UART_SET
} ClockMode;

typedef enum {
    ADJUST_HOUR,
    ADJUST_MINUTE,
    ADJUST_SECOND,
    ADJUST_DAY,
    ADJUST_DATE,
    ADJUST_MONTH,
    ADJUST_YEAR
} AdjustParam;

typedef enum {
    ALARM_HOUR,
    ALARM_MINUTE
} AlarmParam;

//--------C5-bai2--------//
typedef enum {
	UART_SET_IDLE,
	UART_SET_WAIT_HOUR,
	UART_SET_WAIT_MIN,
	UART_SET_SEC,
	UART_SET_DATE,
	UART_SET_MONTH,
	UART_SET_YEAR,
	UART_SET_SAVING
} uart_state;
extern uart_state uart_set_state;

extern ClockMode clock_mode;
extern AdjustParam adjust_param;
extern AlarmParam alarm_param;
//--------C5-bai2-------//

extern volatile uint8_t uart_cmd_ready;
extern char uart_cmd_buffer[32];






void fsm_clock_init();
void fsm_clock_run();

void handle_mode_switch();
void handle_adjust_time_btn();
void handle_set_alarm_btn();

void increment();
void save_time();

void check_alarm();

void display_clock();
void display_view_time();
void display_adjust_time();
void display_set_alarm();
void display_alarm();


void handle_uart_set_mode(void);

#endif /* INC_FSM_CLOCK_H_ */
