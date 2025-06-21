#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "types.h"

/* 全局游戏状态 */
GameState *g_game_state = NULL;

/* 全局变量定义 - 动态网格大小 */
int BOARD_WIDTH = DEFAULT_BOARD_WIDTH;
int BOARD_HEIGHT = DEFAULT_BOARD_HEIGHT;
int WINDOW_WIDTH = 800;
int WINDOW_HEIGHT = 600;

/* 设置网格大小 */
void set_board_size(int width, int height) {
    BOARD_WIDTH = width;
    BOARD_HEIGHT = height;
    WINDOW_WIDTH = width * CELL_SIZE + 200;  /* 额外空间给按钮 */
    WINDOW_HEIGHT = height * CELL_SIZE + 150; /* 额外空间给状态栏 */
}

/* 获取网格宽度 */
int get_board_width(void) {
    return BOARD_WIDTH;
}

/* 获取网格高度 */
int get_board_height(void) {
    return BOARD_HEIGHT;
}

/* 分配二维数组内存 */
static CellType** allocate_board(int width, int height) {
    CellType **board = (CellType**)malloc(height * sizeof(CellType*));
    if (!board) return NULL;
    
    for (int i = 0; i < height; i++) {
        board[i] = (CellType*)malloc(width * sizeof(CellType));
        if (!board[i]) {
            /* 释放已分配的内存 */
            for (int j = 0; j < i; j++) {
                free(board[j]);
            }
            free(board);
            return NULL;
        }
    }
    return board;
}

/* 释放二维数组内存 */
static void free_board(CellType **board, int height) {
    if (!board) return;
    for (int i = 0; i < height; i++) {
        free(board[i]);
    }
    free(board);
}

/* 初始化游戏状态 */
int init_game_state(void) {
    return init_game_state_with_size(BOARD_WIDTH, BOARD_HEIGHT);
}

/* 使用指定大小初始化游戏状态 */
int init_game_state_with_size(int width, int height) {
    /* 设置网格大小 */
    set_board_size(width, height);
    g_game_state = (GameState*)malloc(sizeof(GameState));
    if (!g_game_state) {
        fprintf(stderr, "Error: Unable to allocate memory for game state\n");
        return -1;
    }
    
    /* 分配棋盘内存 */
    g_game_state->board = allocate_board(BOARD_WIDTH, BOARD_HEIGHT);
    if (!g_game_state->board) {
        fprintf(stderr, "Error: Unable to allocate memory for game board\n");
        free(g_game_state);
        g_game_state = NULL;
        return -1;
    }
    
    /* 初始化随机数种子 */
    srand((unsigned int)time(NULL));
    
    /* 初始化棋盘 */
    init_board();
    
    /* 设置玩家初始位置 */
    g_game_state->player_pos.x = 1;
    g_game_state->player_pos.y = 1;
    set_board_cell(1, 1, CELL_PLAYER);
    
    /* 初始化游戏统计 */
    g_game_state->dots_collected = 0;
    g_game_state->moves_count = 0;
    g_game_state->game_over = 0;
    g_game_state->game_won = 0;
    g_game_state->lives = 3;        /* 初始生命值 */
    g_game_state->score = 0;        /* 初始分数 */
    g_game_state->level = 1;        /* 初始关卡 */
    
    /* 生成随机豆子 */
    generate_random_dots(50); /* 生成50个豆子 */
    
    /* 添加一些幽灵 */
    add_ghosts();
    
    /* 添加能量豆 */
    add_power_dots();
    
    /* 计算总豆子数（包括能量豆） */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT || g_game_state->board[i][j] == CELL_POWER_DOT) {
                total++;
            }
        }
    }
    g_game_state->total_dots = total;
    
    return 0;
}

/* 清理游戏状态 */
void cleanup_game_state(void) {
    if (g_game_state) {
        if (g_game_state->board) {
            free_board(g_game_state->board, BOARD_HEIGHT);
        }
        free(g_game_state);
        g_game_state = NULL;
    }
}

/* 重置游戏状态 */
void reset_game_state(void) {
    if (!g_game_state) return;
    
    /* 释放旧的棋盘内存 */
    if (g_game_state->board) {
        free_board(g_game_state->board, BOARD_HEIGHT);
    }
    
    /* 重新分配棋盘内存 */
    g_game_state->board = allocate_board(BOARD_WIDTH, BOARD_HEIGHT);
    if (!g_game_state->board) {
        fprintf(stderr, "Error: Unable to reallocate memory for game board\n");
        return;
    }
    
    /* 重新初始化棋盘 */
    init_board();
    
    /* 重置玩家位置 */
    g_game_state->player_pos.x = 1;
    g_game_state->player_pos.y = 1;
    set_board_cell(1, 1, CELL_PLAYER);
    
    /* 重置游戏统计 */
    g_game_state->dots_collected = 0;
    g_game_state->moves_count = 0;
    g_game_state->game_over = 0;
    g_game_state->game_won = 0;
    g_game_state->lives = 3;        /* 重置生命值 */
    g_game_state->score = 0;        /* 重置分数 */
    g_game_state->level = 1;        /* 重置关卡 */
    
    /* 重新生成豆子 */
    generate_random_dots(50);
    
    /* 重新添加幽灵 */
    add_ghosts();
    
    /* 重新添加能量豆 */
    add_power_dots();
    
    /* 重新计算总豆子数（包括能量豆） */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT || g_game_state->board[i][j] == CELL_POWER_DOT) {
                total++;
            }
        }
    }
    g_game_state->total_dots = total;
}

/* 初始化棋盘 */
void init_board(void) {
    if (!g_game_state) return;
    
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            /* 边界设为墙 */
            if (i == 0 || i == BOARD_HEIGHT-1 || j == 0 || j == BOARD_WIDTH-1) {
                g_game_state->board[i][j] = CELL_WALL;
            } else {
                g_game_state->board[i][j] = CELL_EMPTY;
            }
        }
    }
}

/* 生成随机豆子 */
void generate_random_dots(int num_dots) {
    if (!g_game_state) return;
    
    int placed = 0;
    int attempts = 0;
    int max_attempts = num_dots * 10; /* 防止无限循环 */
    
    while (placed < num_dots && attempts < max_attempts) {
        int x = 1 + rand() % (BOARD_WIDTH - 2);  /* 避开边界 */
        int y = 1 + rand() % (BOARD_HEIGHT - 2); /* 避开边界 */
        
        /* 确保不在玩家位置且该位置为空 */
        if ((x != 1 || y != 1) && g_game_state->board[y][x] == CELL_EMPTY) {
            g_game_state->board[y][x] = CELL_DOT;
            placed++;
        }
        attempts++;
    }
}

/* 清空棋盘单元格 */
void clear_board_cell(int x, int y) {
    if (!g_game_state || !is_within_bounds(x, y)) return;
    g_game_state->board[y][x] = CELL_EMPTY;
}

/* 获取棋盘单元格类型 */
CellType get_board_cell(int x, int y) {
    if (!g_game_state || !is_within_bounds(x, y)) return CELL_WALL;
    return g_game_state->board[y][x];
}

/* 设置棋盘单元格类型 */
void set_board_cell(int x, int y, CellType type) {
    if (!g_game_state || !is_within_bounds(x, y)) return;
    g_game_state->board[y][x] = type;
}

/* 检查移动是否有效 */
int is_valid_move(int x, int y) {
    if (!is_within_bounds(x, y)) return 0;
    if (is_wall_collision(x, y)) return 0;
    return 1;
}

/* 移动玩家到指定位置 */
int move_player_to(int new_x, int new_y) {
    if (!g_game_state || !is_valid_move(new_x, new_y)) {
        return 0; /* 移动失败 */
    }
    
    /* 清除原位置的玩家 */
    int old_x = g_game_state->player_pos.x;
    int old_y = g_game_state->player_pos.y;
    set_board_cell(old_x, old_y, CELL_EMPTY);
    
    /* 检查是否收集豆子 */
    if (check_dot_collection(new_x, new_y)) {
        if (g_game_state->board[new_y][new_x] == CELL_POWER_DOT) {
            collect_power_dot();
        } else {
            collect_dot();
        }
    }
    
    /* 更新玩家位置 */
    update_player_position(new_x, new_y);
    set_board_cell(new_x, new_y, CELL_PLAYER);
    
    /* 增加移动计数 */
    increment_moves();
    
    /* 检查胜利条件 */
    check_win_condition();
    
    return 1; /* 移动成功 */
}

/* 更新玩家位置 */
void update_player_position(int x, int y) {
    if (!g_game_state) return;
    g_game_state->player_pos.x = x;
    g_game_state->player_pos.y = y;
}

/* 获取玩家位置 */
PlayerPosition get_player_position(void) {
    PlayerPosition pos = {0, 0};
    if (g_game_state) {
        pos = g_game_state->player_pos;
    }
    return pos;
}

/* 检查坐标是否在边界内 */
int is_within_bounds(int x, int y) {
    return (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT);
}

/* 检查是否撞墙 */
int is_wall_collision(int x, int y) {
    if (!g_game_state || !is_within_bounds(x, y)) return 1;
    return (g_game_state->board[y][x] == CELL_WALL);
}

/* 检查是否收集豆子 */
int check_dot_collection(int x, int y) {
    if (!g_game_state || !is_within_bounds(x, y)) return 0;
    return (g_game_state->board[y][x] == CELL_DOT || g_game_state->board[y][x] == CELL_POWER_DOT);
}

/* 获取已收集豆子数 */
int get_dots_collected(void) {
    return g_game_state ? g_game_state->dots_collected : 0;
}

/* 获取总豆子数 */
int get_total_dots(void) {
    return g_game_state ? g_game_state->total_dots : 0;
}

/* 获取剩余豆子数 */
int get_remaining_dots(void) {
    if (!g_game_state) return 0;
    return g_game_state->total_dots - g_game_state->dots_collected;
}

/* 获取移动次数 */
int get_moves_count(void) {
    return g_game_state ? g_game_state->moves_count : 0;
}

/* 检查游戏是否结束 */
int is_game_over(void) {
    return g_game_state ? g_game_state->game_over : 0;
}

/* 检查游戏是否获胜 */
int is_game_won(void) {
    return g_game_state ? g_game_state->game_won : 0;
}

/* 增加移动计数 */
void increment_moves(void) {
    if (g_game_state) {
        g_game_state->moves_count++;
    }
}

/* 收集豆子 */
void collect_dot(void) {
    if (g_game_state) {
        g_game_state->dots_collected++;
        g_game_state->score += 10;  /* 每个豆子10分 */
    }
}

/* 收集能量豆 */
void collect_power_dot(void) {
    if (g_game_state) {
        g_game_state->dots_collected++;
        g_game_state->score += 50;  /* 能量豆50分 */
    }
}

/* 检查胜利条件 */
void check_win_condition(void) {
    if (!g_game_state) return;
    
    if (g_game_state->dots_collected >= g_game_state->total_dots) {
        g_game_state->game_won = 1;
        g_game_state->game_over = 1;
        g_game_state->score += 100; /* 胜利奖励100分 */
    }
}

/* 添加幽灵到棋盘 */
void add_ghosts(void) {
    if (!g_game_state) return;
    
    /* 在固定位置添加4个幽灵 */
    int ghost_positions[][2] = {
        {BOARD_WIDTH - 3, 2},     /* 红色幽灵 */
        {BOARD_WIDTH - 3, BOARD_HEIGHT - 3}, /* 蓝色幽灵 */
        {3, BOARD_HEIGHT - 3},    /* 紫色幽灵 */
        {BOARD_WIDTH - 6, 5}      /* 橙色幽灵 */
    };
    
    CellType ghost_types[] = {
        CELL_GHOST_RED,
        CELL_GHOST_BLUE, 
        CELL_GHOST_PURPLE,
        CELL_GHOST_ORANGE
    };
    
    for (int i = 0; i < 4; i++) {
        int x = ghost_positions[i][0];
        int y = ghost_positions[i][1];
        if (is_within_bounds(x, y) && g_game_state->board[y][x] == CELL_EMPTY) {
            g_game_state->board[y][x] = ghost_types[i];
        }
    }
}

/* 添加能量豆到棋盘 */
void add_power_dots(void) {
    if (!g_game_state) return;
    
    /* 在四个角落附近添加能量豆 */
    int power_positions[][2] = {
        {2, 2},
        {BOARD_WIDTH - 3, 2},
        {2, BOARD_HEIGHT - 3},
        {BOARD_WIDTH - 3, BOARD_HEIGHT - 3}
    };
    
    for (int i = 0; i < 4; i++) {
        int x = power_positions[i][0];
        int y = power_positions[i][1];
        if (is_within_bounds(x, y) && g_game_state->board[y][x] == CELL_EMPTY) {
            g_game_state->board[y][x] = CELL_POWER_DOT;
        }
    }
}

/* 更新游戏统计 */
void update_game_statistics(void) {
    if (!g_game_state) return;
    
    /* 重新计算总豆子数（以防有变化） */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT || g_game_state->board[i][j] == CELL_POWER_DOT) {
                total++;
            }
        }
    }
    g_game_state->total_dots = total + g_game_state->dots_collected;
}