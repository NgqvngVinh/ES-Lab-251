#include "light_control.h"

uint8_t light_status = 0;

void lightProcess()
{
    if(button_count[13] == 1){
        light_status = !light_status;
        if(light_status == 1){
            uart_EspSendBytes((uint8_t*)"A", 1);
        }
        else{
            uart_EspSendBytes((uint8_t*)"a", 1);
        }
    }

    if(light_status == 1){
        HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 1);
    }
    else{
        HAL_GPIO_WritePin(OUTPUT_Y0_GPIO_Port, OUTPUT_Y0_Pin, 0);
    }
}

void test_Esp(){
    if(uart_EspCheck() == 0) {
        uart_EspSendBytes((uint8_t*)"o", 1);
        lcd_ShowStr(10, 50, (uint8_t*)"ESP Connect", GREEN, BLACK, 24, 0);
    }
}
