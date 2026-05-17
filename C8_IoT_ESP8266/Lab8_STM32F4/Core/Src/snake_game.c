/*
 * snake_game.c
 *
 *  Created on: 19 thg 11, 2025
 *      Author: PC
 */


#include "snake_game.h"
#include "lcd.h"
#include "touch.h"
#include <stdlib.h> // Cho hàm rand()

// Màu sắc (giả định định nghĩa trong lcd.h, nếu chưa có hãy thêm vào)
//#ifndef WHITE
//#define WHITE 0xFFFF
//#endif
//#ifndef BLACK
//#define BLACK 0x0000
//#endif
//#ifndef RED
//#define RED 0xF800
//#endif
//#ifndef GREEN
//#define GREEN 0x07E0
//#endif
//#ifndef BLUE
//#define BLUE 0x001F
//#endif
//#ifndef YELLOW
//#define YELLOW 0xFFE0
//#endif

// Biến toàn cục của game
static GameState_t currentDataState = GAME_STATE_START;
static Point_t snakeBody[100]; // Độ dài tối đa 100
static uint16_t snakeLength = 3;
static Direction_t currentDir = DIR_RIGHT;
static Point_t food;
static uint8_t gameSpeedCounter = 0;
static uint8_t gameSpeedThreshold = 5; // Tốc độ game (càng nhỏ càng nhanh)

// Hàm nội bộ
static void Draw_Control_Panel(void);
static void Spawn_Food(void);
static void Draw_Block(int x, int y, uint16_t color);
static void Reset_Game(void);

// ---------------------------------------------------------
// Hàm khởi tạo
// ---------------------------------------------------------
void Snake_Init_Game(void) {
    lcd_Clear(BLACK);
    Draw_Control_Panel();
    Reset_Game();

    // Vẽ màn hình Start
    lcd_Fill(40, 80, 200, 160, BLUE);
    lcd_ShowStr(80, 110, "SNAKE", YELLOW, BLUE, 24, 1);
    lcd_ShowStr(65, 140, "Touch to Start", WHITE, BLUE, 16, 1);
}

static void Reset_Game(void) {
    snakeLength = 3;
    currentDir = DIR_RIGHT;
    // Vị trí ban đầu giữa màn hình
    snakeBody[0].x = 5; snakeBody[0].y = 10;
    snakeBody[1].x = 4; snakeBody[1].y = 10;
    snakeBody[2].x = 3; snakeBody[2].y = 10;

    Spawn_Food();
}

static void Spawn_Food(void) {
    // Random vị trí thức ăn trong lưới 24x24
    food.x = rand() % GRID_W;
    food.y = rand() % GRID_H;
}

static void Draw_Block(int x, int y, uint16_t color) {
    lcd_Fill(x * BLOCK_SIZE, y * BLOCK_SIZE,
             (x * BLOCK_SIZE) + BLOCK_SIZE - 1,
             (y * BLOCK_SIZE) + BLOCK_SIZE - 1, color);
}

// Vẽ khu vực điều khiển ở dưới đáy (y > 240)
static void Draw_Control_Panel(void) {
    // Vẽ đường phân cách
    lcd_DrawLine(0, GAME_AREA_H, LCD_WIDTH, GAME_AREA_H, WHITE);

    // Vẽ các nút điều hướng (Giả lập D-PAD)
    // Vùng điều khiển từ y=240 đến y=320 (cao 80 pixel)
    // Chia làm 3 cột: 0-80 (Trái), 80-160 (Lên/Xuống), 160-240 (Phải)

    // Nút LEFT
    lcd_Fill(5, 260, 75, 300, 0x7BEF); // Màu xám nhạt
    lcd_ShowStr(25, 275, "<", BLACK, 0x7BEF, 24, 1);

    // Nút RIGHT
    lcd_Fill(165, 260, 235, 300, 0x7BEF);
    lcd_ShowStr(190, 275, ">", BLACK, 0x7BEF, 24, 1);

    // Nút UP
    lcd_Fill(85, 245, 155, 275, 0x7BEF);
    lcd_ShowStr(115, 250, "^", BLACK, 0x7BEF, 24, 1);

    // Nút DOWN
    lcd_Fill(85, 285, 155, 315, 0x7BEF);
    lcd_ShowStr(115, 290, "v", BLACK, 0x7BEF, 24, 1);
}

// Xử lý input từ màn hình cảm ứng
static void Handle_Input(void) {
    if (!touch_IsTouched()) return;

    uint16_t tx = touch_GetX();
    uint16_t ty = touch_GetY();

    // Xử lý trạng thái START hoặc GAMEOVER
    if (currentDataState != GAME_STATE_PLAYING) {
        // Chạm bất kỳ đâu để bắt đầu lại (debounce nhẹ bằng cách chờ nhả tay ở logic chính)
        if (currentDataState == GAME_STATE_GAMEOVER) {
             // Xóa bảng Game Over
             lcd_Fill(0, 0, LCD_WIDTH, GAME_AREA_H, BLACK);
        } else {
             // Xóa bảng Start
             lcd_Fill(40, 80, 200, 160, BLACK);
        }
        Reset_Game();
        currentDataState = GAME_STATE_PLAYING;
        return;
    }

    // Xử lý khi đang chơi: Điều khiển hướng
    // Chỉ nhận cảm ứng ở vùng điều khiển (y > 240)
    if (ty > 240) {
        if (tx < 80) { // Vùng Trái
            if (currentDir != DIR_RIGHT) currentDir = DIR_LEFT;
        } else if (tx > 160) { // Vùng Phải
            if (currentDir != DIR_LEFT) currentDir = DIR_RIGHT;
        } else { // Vùng Giữa
            if (ty < 280) { // Nửa trên -> UP
                if (currentDir != DIR_DOWN) currentDir = DIR_UP;
            } else { // Nửa dưới -> DOWN
                if (currentDir != DIR_UP) currentDir = DIR_DOWN;
            }
        }
    }
}

// Logic di chuyển và va chạm
static void Update_Game_Logic(void) {
    Point_t nextHead = snakeBody[0];

    switch (currentDir) {
        case DIR_UP:    nextHead.y--; break;
        case DIR_DOWN:  nextHead.y++; break;
        case DIR_LEFT:  nextHead.x--; break;
        case DIR_RIGHT: nextHead.x++; break;
    }

    // 1. Kiểm tra va chạm tường
    if (nextHead.x < 0 || nextHead.x >= GRID_W || nextHead.y < 0 || nextHead.y >= GRID_H) {
        currentDataState = GAME_STATE_GAMEOVER;
        return;
    }

    // 2. Kiểm tra cắn vào thân
    for (int i = 0; i < snakeLength; i++) {
        if (nextHead.x == snakeBody[i].x && nextHead.y == snakeBody[i].y) {
            currentDataState = GAME_STATE_GAMEOVER;
            return;
        }
    }

    // 3. Di chuyển rắn (Shift body)
    // Xóa đuôi cũ trên màn hình
    Draw_Block(snakeBody[snakeLength - 1].x, snakeBody[snakeLength - 1].y, BLACK);

    for (int i = snakeLength - 1; i > 0; i--) {
        snakeBody[i] = snakeBody[i - 1];
    }
    snakeBody[0] = nextHead;

    // 4. Kiểm tra ăn mồi
    if (nextHead.x == food.x && nextHead.y == food.y) {
        if (snakeLength < 100) {
            snakeLength++;
            // (Đuôi mới sẽ tự động được vẽ ở frame sau)
        }
        Spawn_Food();
    }

    // 5. Vẽ lại đầu rắn và thức ăn
    Draw_Block(snakeBody[0].x, snakeBody[0].y, GREEN); // Đầu rắn
    Draw_Block(snakeBody[1].x, snakeBody[1].y, YELLOW); // Cổ rắn (để thấy chuyển động)
    Draw_Block(food.x, food.y, RED); // Thức ăn
}

// Hàm Loop chính, gọi trong main while(1)
// Sử dụng cờ timer từ bên ngoài để định thời
//extern uint8_t flag_timer2; // Lấy biến cờ timer từ main.c

void Snake_Main_Loop(void) {
    // Quét cảm ứng liên tục để nhạy
    touch_Scan();
    Handle_Input();

    // Logic game chạy theo Timer (50ms mỗi lần set cờ)
    if (flag_timer2) {
        flag_timer2 = 0;

        if (currentDataState == GAME_STATE_PLAYING) {
            gameSpeedCounter++;
            // Cập nhật vị trí rắn mỗi 5 ticks (5 * 50ms = 250ms)
            if (gameSpeedCounter >= gameSpeedThreshold) {
                gameSpeedCounter = 0;
                Update_Game_Logic();
            }
        }
        else if (currentDataState == GAME_STATE_GAMEOVER) {
             // Vẽ màn hình Game Over 1 lần rồi thôi
             static uint8_t drawn = 0;
             if (!drawn) {
                 lcd_Fill(40, 80, 200, 180, RED);
                 lcd_ShowStr(65, 100, "GAME OVER", WHITE, RED, 24, 1);
                 lcd_ShowStr(60, 140, "Score: ", WHITE, RED, 16, 1);
                 lcd_ShowIntNum(120, 140, snakeLength - 3, 3, YELLOW, RED, 16);
                 drawn = 1;
             }
             // Reset drawn flag khi chuyển trạng thái ở Handle_Input
             if (touch_IsTouched()) drawn = 0;
        }
    }
}
