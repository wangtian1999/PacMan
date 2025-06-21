#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "types.h"

/* 全局游戏状态 */
GameState *g_game_state = NULL;

/* 初始化游戏状态 */
int init_game_state(void) {
    g_game_state = (GameState*)malloc(sizeof(GameState));
    if (!g_game_state) {
        fprintf(stderr, "Error: Unable to allocate memory for game state\n");
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
    
    /* 生成随机豆子 */
    generate_random_dots(50); /* 生成50个豆子 */
    
    /* 计算总豆子数 */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT) {
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
        free(g_game_state);
        g_game_state = NULL;
    }
}

/* 重置游戏状态 */
void reset_game_state(void) {
    if (!g_game_state) return;
    
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
    
    /* 重新生成豆子 */
    generate_random_dots(50);
    
    /* 重新计算总豆子数 */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT) {
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
        collect_dot();
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
    return (g_game_state->board[y][x] == CELL_DOT);
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
    }
}

/* 检查胜利条件 */
void check_win_condition(void) {
    if (!g_game_state) return;
    
    if (g_game_state->dots_collected >= g_game_state->total_dots) {
        g_game_state->game_won = 1;
        g_game_state->game_over = 1;
    }
}

/* 更新游戏统计 */
void update_game_statistics(void) {
    if (!g_game_state) return;
    
    /* 重新计算总豆子数（以防有变化） */
    int total = 0;
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_DOT) {
                total++;
            }
        }
    }
    g_game_state->total_dots = total + g_game_state->dots_collected;
}