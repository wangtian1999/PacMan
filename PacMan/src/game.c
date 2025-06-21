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
    g_game_state->auto_move_direction = DIR_RIGHT; /* 默认自动移动方向 */
    g_game_state->auto_move_enabled = 0;           /* 默认关闭自动移动 */
    g_game_state->last_move_time = 0;
    
    /* 豆子已在init_board中生成，无需额外生成 */
    
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
    g_game_state->auto_move_direction = DIR_RIGHT; /* 重置自动移动方向 */
    g_game_state->auto_move_enabled = 0;           /* 重置自动移动状态 */
    g_game_state->last_move_time = 0;
    
    /* 豆子已在init_board中生成，无需额外生成 */
    
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
    
    /* 首先将所有格子设为空 */
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            g_game_state->board[i][j] = CELL_EMPTY;
        }
    }
    
    /* 生成四周边界墙壁 */
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (i == 0 || i == BOARD_HEIGHT-1 || j == 0 || j == BOARD_WIDTH-1) {
                g_game_state->board[i][j] = CELL_WALL;
            }
        }
    }
    
    /* 计算内部区域的格子数，并随机生成内部墙壁 */
    int inner_width = BOARD_WIDTH - 2;
    int inner_height = BOARD_HEIGHT - 2;
    int inner_cells = inner_width * inner_height;
    int inner_wall_count = (int)(inner_cells * 0.2); /* 内部墙壁数量为内部区域的20% */
    
    /* 确保内部墙壁数量不超过内部可用格子数 */
    if (inner_wall_count > inner_cells - 1) { /* 至少保留玩家位置 */
        inner_wall_count = inner_cells - 1;
    }
    
    /* 在内部区域随机放置墙壁，确保不创建封闭区域 */
    int placed_walls = 0;
    int max_attempts = inner_wall_count * 10; /* 防止无限循环 */
    int attempts = 0;
    
    while (placed_walls < inner_wall_count && attempts < max_attempts) {
        int x = 1 + rand() % inner_width;  /* 内部区域x坐标 */
        int y = 1 + rand() % inner_height; /* 内部区域y坐标 */
        
        /* 确保不在玩家初始位置(1,1)且该位置不是墙 */
        if ((x != 1 || y != 1) && g_game_state->board[y][x] != CELL_WALL) {
            /* 临时放置墙壁 */
            g_game_state->board[y][x] = CELL_WALL;
            
            /* 检查是否会创建封闭区域 - 简单策略：确保周围至少有2个方向可通行 */
            int passable_directions = 0;
            int dx[] = {0, 1, 0, -1}; /* 右、下、左、上 */
            int dy[] = {1, 0, -1, 0};
            
            for (int dir = 0; dir < 4; dir++) {
                int nx = x + dx[dir];
                int ny = y + dy[dir];
                if (nx >= 1 && nx < BOARD_WIDTH-1 && ny >= 1 && ny < BOARD_HEIGHT-1) {
                    if (g_game_state->board[ny][nx] != CELL_WALL) {
                        passable_directions++;
                    }
                }
            }
            
            /* 如果周围可通行方向少于2个，撤销墙壁放置 */
            if (passable_directions < 2) {
                g_game_state->board[y][x] = CELL_EMPTY;
            } else {
                placed_walls++;
            }
        }
        attempts++;
    }
    
    /* 先在所有非墙壁位置放置豆子 */
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (g_game_state->board[i][j] == CELL_EMPTY && (j != 1 || i != 1)) {
                g_game_state->board[i][j] = CELL_DOT;
            }
        }
    }
    
    /* 使用洪水填充算法标记从玩家位置可到达的区域 */
    int visited[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            visited[i][j] = 0;
        }
    }
    
    /* 简单的递归洪水填充函数 */
    void flood_fill(int x, int y) {
        if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) return;
        if (visited[y][x] || g_game_state->board[y][x] == CELL_WALL) return;
        
        visited[y][x] = 1;
        flood_fill(x+1, y);
        flood_fill(x-1, y);
        flood_fill(x, y+1);
        flood_fill(x, y-1);
    }
    
    /* 从玩家起始位置开始洪水填充 */
    flood_fill(1, 1);
    
    /* 清除不可到达区域的豆子 */
    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (!visited[i][j] && (g_game_state->board[i][j] == CELL_DOT || g_game_state->board[i][j] == CELL_POWER_DOT)) {
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
    
    CellType ghost_types[] = {
        CELL_GHOST_RED,
        CELL_GHOST_BLUE, 
        CELL_GHOST_PURPLE,
        CELL_GHOST_ORANGE
    };
    
    /* 随机放置4个幽灵 */
    for (int i = 0; i < 4; i++) {
        int placed = 0;
        int attempts = 0;
        int max_attempts = 100;
        
        while (!placed && attempts < max_attempts) {
            int x = rand() % BOARD_WIDTH;
            int y = rand() % BOARD_HEIGHT;
            
            /* 确保不在玩家位置且该位置是豆子 */
            if ((x != 1 || y != 1) && g_game_state->board[y][x] == CELL_DOT) {
                g_game_state->board[y][x] = ghost_types[i];
                placed = 1;
            }
            attempts++;
        }
    }
}

/* 添加能量豆到棋盘 */
void add_power_dots(void) {
    if (!g_game_state) return;
    
    /* 随机放置4个能量豆 */
    for (int i = 0; i < 4; i++) {
        int placed = 0;
        int attempts = 0;
        int max_attempts = 100;
        
        while (!placed && attempts < max_attempts) {
            int x = rand() % BOARD_WIDTH;
            int y = rand() % BOARD_HEIGHT;
            
            /* 确保不在玩家位置且该位置是豆子 */
            if ((x != 1 || y != 1) && g_game_state->board[y][x] == CELL_DOT) {
                g_game_state->board[y][x] = CELL_POWER_DOT;
                placed = 1;
            }
            attempts++;
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