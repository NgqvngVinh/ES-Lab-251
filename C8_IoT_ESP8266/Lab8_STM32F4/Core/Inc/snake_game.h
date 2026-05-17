/*
 * snake_game.h
 *
 *  Created on: 19 thg 11, 2025
 *      Author: PC
 */

#ifndef INC_SNAKE_GAME_H_
#define INC_SNAKE_GAME_H_

#include "main.h"

// Kích thước màn hình và block
#define LCD_WIDTH       240
#define LCD_HEIGHT      320
#define GAME_AREA_H     240 // Phần trên dùng để chơi (240x240)
#define BLOCK_SIZE      10  // Kích thước 1 đốt rắn (10x10 pixel)

// Định nghĩa lưới (Grid)
#define GRID_W          (LCD_WIDTH / BLOCK_SIZE)      // 24 cột
#define GRID_H          (GAME_AREA_H / BLOCK_SIZE)    // 24 hàng

// Trạng thái game
typedef enum {
    GAME_STATE_START,
    GAME_STATE_PLAYING,
    GAME_STATE_GAMEOVER
} GameState_t;

// Hướng di chuyển
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
} Direction_t;

// Cấu trúc tọa độ
typedef struct {
    int16_t x;
    int16_t y;
} Point_t;

// Các hàm public
void Snake_Init_Game(void);
void Snake_Main_Loop(void);



#endif /* INC_SNAKE_GAME_H_ */
