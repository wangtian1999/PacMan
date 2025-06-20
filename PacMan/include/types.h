#ifndef TYPES_H
#define TYPES_H

/* 游戏常量定义 */
#define BOARD_WIDTH 20
#define BOARD_HEIGHT 15
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define CELL_SIZE 30

/* 移动方向枚举 */
typedef enum {
    DIR_UP = 0,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_COUNT
} Direction;

/* 棋盘单元格类型 */
typedef enum {
    CELL_EMPTY = 0,
    CELL_WALL,
    CELL_DOT,
    CELL_PLAYER
} CellType;

/* 玩家位置结构体 */
typedef struct {
    int x;
    int y;
} PlayerPosition;

/* 游戏状态结构体 */
typedef struct {
    CellType board[BOARD_HEIGHT][BOARD_WIDTH];
    PlayerPosition player_pos;
    int dots_collected;
    int total_dots;
    int moves_count;
    int game_over;
    int game_won;
} GameState;

#endif /* TYPES_H */