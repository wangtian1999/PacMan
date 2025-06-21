#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "types.h"

/* 算法类型枚举 */
typedef enum {
    ALGO_NONE = 0,
    ALGO_RANDOM,
    ALGO_ZIGZAG,
    ALGO_DFS
} AlgorithmType;

/* 幽灵位置结构体 */
typedef struct {
    int x, y;
    CellType type;
    CellType original_cell; /* 幽灵下方的原始单元格类型 */
    Direction last_direction;
    int zigzag_steps;
    Direction zigzag_direction;
} GhostInfo;

/* 全局算法状态 */
static AlgorithmType current_algorithm = ALGO_NONE;
static GhostInfo ghosts[4];
static int ghost_count = 0;
static long last_move_time = 0;
static int move_interval = 500; /* 幽灵移动间隔（毫秒） */

/* 获取当前时间（毫秒） - 简化版本 */
static long get_current_time_ms(void) {
    static int counter = 0;
    counter += 100; /* 每次调用增加100ms，模拟定时器间隔 */
    return counter;
}

/* 初始化幽灵信息 */
static void init_ghosts_info(void) {
    ghost_count = 0;
    
    printf("Scanning board for ghosts...\n");
    printf("Board size: %dx%d\n", get_board_width(), get_board_height());
    
    for (int y = 0; y < get_board_height(); y++) {
        for (int x = 0; x < get_board_width(); x++) {
            CellType cell = get_board_cell(x, y);
            if (cell == CELL_GHOST_RED || cell == CELL_GHOST_BLUE ||
                cell == CELL_GHOST_PURPLE || cell == CELL_GHOST_ORANGE) {
                printf("Found ghost at (%d,%d), type: %d\n", x, y, cell);
                if (ghost_count < 4) {
                    ghosts[ghost_count].x = x;
                    ghosts[ghost_count].y = y;
                    ghosts[ghost_count].type = cell;
                    ghosts[ghost_count].original_cell = CELL_EMPTY; /* 假设幽灵初始在空地上 */
                    ghosts[ghost_count].last_direction = DIR_RIGHT;
                    ghosts[ghost_count].zigzag_steps = 0;
                    ghosts[ghost_count].zigzag_direction = DIR_RIGHT;
                    printf("Added ghost %d at position (%d,%d)\n", ghost_count, x, y);
                    ghost_count++;
                } else {
                    printf("Warning: Too many ghosts found, ignoring ghost at (%d,%d)\n", x, y);
                }
            }
        }
    }
    
    printf("Ghost initialization complete. Total ghosts: %d\n", ghost_count);
}

/* 检查位置是否有效（幽灵可以移动到的位置） */
static int is_valid_ghost_move(int x, int y) {
    if (!is_within_bounds(x, y)) return 0;
    
    CellType cell = get_board_cell(x, y);
    /* 幽灵可以移动到空地、豆子、能量豆和玩家位置，但不能移动到墙壁或其他幽灵 */
    return (cell == CELL_EMPTY || cell == CELL_DOT || 
            cell == CELL_POWER_DOT || cell == CELL_PLAYER);
}

/* 获取幽灵的有效移动方向 */
static int get_ghost_valid_directions(int ghost_x, int ghost_y, Direction *valid_dirs) {
    int count = 0;
    Direction dirs[4] = {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};
    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};
    
    for (int i = 0; i < 4; i++) {
        int new_x = ghost_x + dx[i];
        int new_y = ghost_y + dy[i];
        
        if (is_valid_ghost_move(new_x, new_y)) {
            valid_dirs[count++] = dirs[i];
        }
    }
    
    return count;
}

/* Random算法 - 随机移动幽灵 */
static Direction random_ghost_algorithm(int ghost_index) {
    Direction valid_dirs[4];
    int count = get_ghost_valid_directions(ghosts[ghost_index].x, ghosts[ghost_index].y, valid_dirs);
    
    if (count == 0) {
        return ghosts[ghost_index].last_direction;
    }
    
    int index = rand() % count;
    return valid_dirs[index];
}

/* Zig-Zag算法 - 之字形移动 */
static Direction zigzag_ghost_algorithm(int ghost_index) {
    Direction valid_dirs[4];
    int count = get_ghost_valid_directions(ghosts[ghost_index].x, ghosts[ghost_index].y, valid_dirs);
    
    if (count == 0) {
        return ghosts[ghost_index].last_direction;
    }
    
    /* 检查当前方向是否有效 */
    int current_valid = 0;
    for (int i = 0; i < count; i++) {
        if (valid_dirs[i] == ghosts[ghost_index].zigzag_direction) {
            current_valid = 1;
            break;
        }
    }
    
    /* 如果当前方向有效且未达到最大步数，继续当前方向 */
    if (current_valid && ghosts[ghost_index].zigzag_steps < 5) {
        ghosts[ghost_index].zigzag_steps++;
        return ghosts[ghost_index].zigzag_direction;
    }
    
    /* 需要改变方向 */
    ghosts[ghost_index].zigzag_steps = 1;
    
    /* 尝试垂直方向切换 */
    Direction new_direction;
    if (ghosts[ghost_index].zigzag_direction == DIR_RIGHT || 
        ghosts[ghost_index].zigzag_direction == DIR_LEFT) {
        new_direction = (rand() % 2) ? DIR_UP : DIR_DOWN;
    } else {
        new_direction = (rand() % 2) ? DIR_LEFT : DIR_RIGHT;
    }
    
    /* 检查新方向是否有效 */
    for (int i = 0; i < count; i++) {
        if (valid_dirs[i] == new_direction) {
            ghosts[ghost_index].zigzag_direction = new_direction;
            return new_direction;
        }
    }
    
    /* 如果新方向无效，随机选择一个有效方向 */
    int index = rand() % count;
    ghosts[ghost_index].zigzag_direction = valid_dirs[index];
    return valid_dirs[index];
}

/* DFS算法 - 追踪玩家 */
static Direction dfs_ghost_algorithm(int ghost_index) {
    PlayerPosition player_pos = get_player_position();
    int ghost_x = ghosts[ghost_index].x;
    int ghost_y = ghosts[ghost_index].y;
    
    Direction valid_dirs[4];
    int count = get_ghost_valid_directions(ghost_x, ghost_y, valid_dirs);
    
    if (count == 0) {
        return ghosts[ghost_index].last_direction;
    }
    
    /* 计算到玩家的距离，选择最接近玩家的方向 */
    Direction best_dir = valid_dirs[0];
    int min_distance = 9999;
    
    int dx[4] = {0, 0, -1, 1};
    int dy[4] = {-1, 1, 0, 0};
    
    for (int i = 0; i < count; i++) {
        Direction dir = valid_dirs[i];
        int dir_index = (dir == DIR_UP) ? 0 : (dir == DIR_DOWN) ? 1 : 
                       (dir == DIR_LEFT) ? 2 : 3;
        
        int new_x = ghost_x + dx[dir_index];
        int new_y = ghost_y + dy[dir_index];
        
        int distance = abs(new_x - player_pos.x) + abs(new_y - player_pos.y);
        
        if (distance < min_distance) {
            min_distance = distance;
            best_dir = dir;
        }
    }
    
    return best_dir;
}

/* 移动单个幽灵 */
static void move_ghost(int ghost_index) {
    if (ghost_index >= ghost_count) return;
    
    Direction move_dir;
    
    switch (current_algorithm) {
        case ALGO_RANDOM:
            move_dir = random_ghost_algorithm(ghost_index);
            break;
        case ALGO_ZIGZAG:
            move_dir = zigzag_ghost_algorithm(ghost_index);
            break;
        case ALGO_DFS:
            move_dir = dfs_ghost_algorithm(ghost_index);
            break;
        default:
            return;
    }
    
    /* 计算新位置 */
    int dx = (move_dir == DIR_RIGHT) ? 1 : (move_dir == DIR_LEFT) ? -1 : 0;
    int dy = (move_dir == DIR_DOWN) ? 1 : (move_dir == DIR_UP) ? -1 : 0;
    
    int old_x = ghosts[ghost_index].x;
    int old_y = ghosts[ghost_index].y;
    int new_x = old_x + dx;
    int new_y = old_y + dy;
    
    /* 验证移动是否有效 */
    if (!is_valid_ghost_move(new_x, new_y)) {
        return;
    }
    
    /* 检查新位置是否与玩家碰撞 */
    PlayerPosition player_pos = get_player_position();
    if (new_x == player_pos.x && new_y == player_pos.y) {
        /* 幽灵抓到玩家，触发游戏结束 */
        handle_player_death();
        return;
    }
    
    /* 移动幽灵 */
    CellType new_cell = get_board_cell(new_x, new_y);
    
    /* 恢复旧位置的原始内容 */
    set_board_cell(old_x, old_y, ghosts[ghost_index].original_cell);
    
    /* 保存新位置的原始内容 */
    ghosts[ghost_index].original_cell = new_cell;
    
    /* 设置新位置为幽灵 */
    set_board_cell(new_x, new_y, ghosts[ghost_index].type);
    
    /* 更新幽灵信息 */
    ghosts[ghost_index].x = new_x;
    ghosts[ghost_index].y = new_y;
    ghosts[ghost_index].last_direction = move_dir;
}

/* 设置当前算法 */
void set_algorithm(int algorithm_type) {
    printf("set_algorithm called with type: %d\n", algorithm_type);
    
    current_algorithm = (AlgorithmType)algorithm_type;
    printf("Current algorithm set to: %d\n", current_algorithm);
    
    /* 初始化幽灵信息 */
    printf("Initializing ghosts info...\n");
    init_ghosts_info();
    printf("Found %d ghosts on board\n", ghost_count);
    
    /* 重置算法状态 */
    for (int i = 0; i < ghost_count; i++) {
        ghosts[i].zigzag_steps = 0;
        ghosts[i].zigzag_direction = DIR_RIGHT;
        printf("Ghost %d at position (%d,%d), type: %d\n", i, ghosts[i].x, ghosts[i].y, ghosts[i].type);
    }
    
    /* 初始化随机数种子 */
    srand((unsigned int)time(NULL));
    
    /* 重置移动时间 */
    last_move_time = get_current_time_ms();
    printf("Algorithm setup complete. Move interval: %d ms\n", move_interval);
}

/* 更新幽灵移动（由定时器调用） */
void update_ghost_movement(void) {
    if (current_algorithm == ALGO_NONE) {
        printf("update_ghost_movement: No algorithm enabled\n");
        return;
    }
    
    long current_time = get_current_time_ms();
    long time_diff = current_time - last_move_time;
    
    printf("update_ghost_movement: current_time=%ld, last_move_time=%ld, diff=%ld, interval=%d\n", 
           current_time, last_move_time, time_diff, move_interval);
    
    /* 检查是否到了移动时间 */
    if (time_diff >= move_interval) {
        /* 重新初始化幽灵信息（防止幽灵丢失） */
        init_ghosts_info();
        
        printf("Moving %d ghosts with algorithm %d\n", ghost_count, current_algorithm);
        
        /* 移动所有幽灵 */
        for (int i = 0; i < ghost_count; i++) {
            printf("Moving ghost %d from (%d,%d)\n", i, ghosts[i].x, ghosts[i].y);
            move_ghost(i);
            printf("Ghost %d moved to (%d,%d)\n", i, ghosts[i].x, ghosts[i].y);
        }
        
        last_move_time = current_time;
    }
}

/* 检查是否启用了算法 */
int is_algorithm_enabled(void) {
    return current_algorithm != ALGO_NONE;
}

/* 停止算法 */
void stop_algorithm(void) {
    current_algorithm = ALGO_NONE;
    ghost_count = 0;
}

/* 获取当前算法名称 */
const char* get_algorithm_name(void) {
    switch (current_algorithm) {
        case ALGO_RANDOM:
            return "Random Ghost";
        case ALGO_ZIGZAG:
            return "Zig-Zag Ghost";
        case ALGO_DFS:
            return "Hunting Ghost";
        default:
            return "None";
    }
}

/* 设置幽灵移动间隔 */
void set_ghost_move_interval(int interval_ms) {
    move_interval = interval_ms;
}

/* 获取幽灵移动间隔 */
int get_ghost_move_interval(void) {
    return move_interval;
}