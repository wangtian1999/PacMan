#ifndef TYPES_H
#define TYPES_H

/* 游戏常量定义 */
#define DEFAULT_BOARD_WIDTH 20
#define DEFAULT_BOARD_HEIGHT 15
#define MAX_BOARD_WIDTH 50
#define MAX_BOARD_HEIGHT 40
#define MIN_BOARD_WIDTH 10
#define MIN_BOARD_HEIGHT 8
#define CELL_SIZE 30

/* 全局变量声明 - 动态网格大小 */
extern int BOARD_WIDTH;
extern int BOARD_HEIGHT;
extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

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
    CELL_PLAYER,
    CELL_GHOST_RED,    /* 红色幽灵 */
    CELL_GHOST_BLUE,   /* 蓝色幽灵 */
    CELL_GHOST_PURPLE, /* 紫色幽灵 */
    CELL_GHOST_ORANGE, /* 橙色幽灵 */
    CELL_POWER_DOT,    /* 能量豆 */
    CELL_FRUIT         /* 水果奖励 */
} CellType;

/* 玩家位置结构体 */
typedef struct {
    int x;
    int y;
} PlayerPosition;

/* 游戏状态结构体 */
typedef struct {
    CellType **board;  /* 改为动态分配的二维数组 */
    PlayerPosition player_pos;
    int dots_collected;
    int total_dots;
    int moves_count;
    int game_over;
    int game_won;
    int lives;         /* 生命值 */
    int score;         /* 分数 */
    int level;         /* 关卡 */
} GameState;

#endif /* TYPES_H */