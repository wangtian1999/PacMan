#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libsx.h>
#include "gui.h"
#include "game.h"
#include "types.h"
#include "algorithms.h"
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif

/* 获取当前时间毫秒数 */
long get_current_time_ms(void) {
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

/* 定时器回调函数 */
void timer_callback(void *data) {
    /* 避免编译器警告 */
    (void)data;
    
    /* 检查游戏状态 */
    if (!g_game_state || is_game_over()) {
        AddTimeOut(100, timer_callback, NULL);
        return;
    }
    
    /* 幽灵自动移动逻辑 */
    if (is_algorithm_enabled()) {
        update_ghost_movement();
    } else {
        /* 传统自动移动处理（玩家） */
        process_auto_move();
    }
    
    /* 定期更新显示 */
    update_display();
    
    /* 重新设置定时器，实现循环调用 */
    AddTimeOut(100, timer_callback, NULL);
}

/* 全局GUI组件 */
Widget g_main_window;
Widget g_drawing_area;
Widget g_status_label;

/* 颜色定义 */
static int color_black, color_white, color_blue, color_yellow, color_red;
static int color_pink, color_cyan, color_purple, color_orange;
static int color_dark_blue, color_light_blue, color_green;

/* 初始化GUI */
int init_gui(int argc, char *argv[]) {
    Widget button_up, button_down, button_left, button_right;
    Widget button_rejouer, button_aide, button_quit;
    Widget button_random, button_zigzag, button_dfs, button_stop_algo;
    
    /* 初始化libsx */
    if (OpenDisplay(argc, argv) == 0) {
        fprintf(stderr, "错误: 无法打开显示\n");
        return -1;
    }
    
    /* 创建主窗口 */
    g_main_window = MakeWindow("PacMan Game", NULL, 0);
    if (!g_main_window) {
        fprintf(stderr, "错误: 无法创建主窗口\n");
        return -1;
    }
    
    /* 初始化颜色 */
    color_black = GetNamedColor("black");
    color_white = GetNamedColor("white");
    color_blue = GetNamedColor("blue");
    color_yellow = GetNamedColor("yellow");
    color_red = GetNamedColor("red");
    color_pink = GetNamedColor("pink");
    color_cyan = GetNamedColor("cyan");
    color_purple = GetNamedColor("purple");
    color_orange = GetNamedColor("orange");
    color_dark_blue = GetNamedColor("navy");
    color_light_blue = GetNamedColor("lightblue");
    color_green = GetNamedColor("green");
    
    /* 检查颜色是否成功初始化 */
    if (color_black == -1 || color_white == -1 || color_blue == -1 || 
        color_yellow == -1 || color_red == -1 || color_pink == -1) {
        fprintf(stderr, "警告: 某些颜色初始化失败\n");
    }
    
    /* 创建绘图区域 - 使用动态网格大小 */
    g_drawing_area = MakeDrawArea(get_board_width() * CELL_SIZE, 
                                  get_board_height() * CELL_SIZE, 
                                  draw_board, NULL);
    if (!g_drawing_area) {
        fprintf(stderr, "错误: 无法创建绘图区域\n");
        return -1;
    }
    
    /* 创建方向控制按钮 */
    button_up = MakeButton("Up", button_up_callback, NULL);
    button_down = MakeButton("Down", button_down_callback, NULL);
    button_left = MakeButton("Left", button_left_callback, NULL);
    button_right = MakeButton("Right", button_right_callback, NULL);
    
    if (!button_up || !button_down || !button_left || !button_right) {
        fprintf(stderr, "错误: 方向按钮创建失败\n");
        return -1;
    }
    
    /* 创建功能按钮 */
    button_rejouer = MakeButton("Restart", button_rejouer_callback, NULL);
    button_aide = MakeButton("Help", button_aide_callback, NULL);
    button_quit = MakeButton("Quit", button_quit_callback, NULL);
    
    if (!button_rejouer || !button_aide || !button_quit) {
        fprintf(stderr, "错误: 功能按钮创建失败\n");
        return -1;
    }
    
    /* 创建幽灵算法按钮 */
    button_random = MakeButton("Random Ghost", button_random_callback, NULL);
    button_zigzag = MakeButton("ZigZag Ghost", button_zigzag_callback, NULL);
    button_dfs = MakeButton("Hunt Ghost", button_dfs_callback, NULL);
    button_stop_algo = MakeButton("Stop Ghost", button_stop_algo_callback, NULL);
    
    if (!button_random || !button_zigzag || !button_dfs || !button_stop_algo) {
        fprintf(stderr, "错误: 算法按钮创建失败\n");
        return -1;
    }
    
    /* 创建状态显示标签 */
    g_status_label = MakeLabel("Moves: 0  Collected: 0  Remaining: 0");
    
    if (!g_status_label) {
        fprintf(stderr, "错误: 状态标签创建失败\n");
        return -1;
    }
    
    /* 设置布局 - 添加错误检查 */
    /* 检查所有widget是否有效 */
    if (!g_drawing_area || !button_up || !button_left || !button_down || !button_right ||
        !button_rejouer || !button_aide || !button_quit || !g_status_label ||
        !button_random || !button_zigzag || !button_dfs || !button_stop_algo) {
        fprintf(stderr, "错误: 某些GUI组件创建失败\n");
        return -1;
    }
    
    /* 设置方向按钮布局 */
    /* 所有方向按钮都直接相对于绘图区域布局 */
    if (button_left && g_drawing_area) {
        SetWidgetPos(button_left, PLACE_UNDER, g_drawing_area, NO_CARE, NULL);
    }
    
    if (button_down && g_drawing_area) {
        SetWidgetPos(button_down, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_left);
    }
    
    if (button_up && g_drawing_area) {
        SetWidgetPos(button_up, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_down);
    }
    
    if (button_right && g_drawing_area) {
        SetWidgetPos(button_right, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_up);
    }
    
    /* 功能按钮布局 */
    /* 重新开始按钮在左按钮下方 */
    if (button_rejouer && button_left) {
        SetWidgetPos(button_rejouer, PLACE_UNDER, button_left, NO_CARE, NULL);
    }
    
    /* 帮助按钮在左按钮下方，重新开始按钮右侧 */
    if (button_aide && button_left) {
        SetWidgetPos(button_aide, PLACE_UNDER, button_left, PLACE_RIGHT, button_rejouer);
    }
    
    /* 退出按钮在左按钮下方，帮助按钮右侧 */
    if (button_quit && button_left) {
        SetWidgetPos(button_quit, PLACE_UNDER, button_left, PLACE_RIGHT, button_aide);
    }
    
    /* 算法按钮布局 - 在功能按钮下方 */
    if (button_random && button_rejouer) {
        SetWidgetPos(button_random, PLACE_UNDER, button_rejouer, NO_CARE, NULL);
    }
    
    if (button_zigzag && button_rejouer) {
        SetWidgetPos(button_zigzag, PLACE_UNDER, button_rejouer, PLACE_RIGHT, button_random);
    }
    
    if (button_dfs && button_rejouer) {
        SetWidgetPos(button_dfs, PLACE_UNDER, button_rejouer, PLACE_RIGHT, button_zigzag);
    }
    
    if (button_stop_algo && button_rejouer) {
        SetWidgetPos(button_stop_algo, PLACE_UNDER, button_rejouer, PLACE_RIGHT, button_dfs);
    }
    
    /* 状态标签布局 - 在算法按钮下方 */
    if (g_status_label && button_random) {
        SetWidgetPos(g_status_label, PLACE_UNDER, button_random, NO_CARE, NULL);
    }
    
    /* 设置键盘事件处理 */
    SetKeypressCB(g_main_window, key_press_callback);
    
    /* Linux/X11特定设置 - 确保键盘焦点 */
    SetWidgetState(g_main_window, 1);
    /* 尝试设置输入焦点 */
    if (g_drawing_area) {
        SetKeypressCB(g_drawing_area, key_press_callback);
    }
    
    /* 获取标准颜色 - libsx要求在ShowDisplay之前调用 */
    GetStandardColors();
    
    /* 显示窗口 */
    ShowDisplay();
    
    /* 设置定时器，每100毫秒检查一次自动移动 */
    AddTimeOut(100, timer_callback, NULL);
    
    /* 确保窗口获得键盘焦点 - Linux/X11增强版 */
    SetWidgetState(g_main_window, 1); /* 激活窗口 */
    
    /* Linux特定：强制获取键盘焦点 */
    /* 尝试多种方式确保键盘事件被捕获 */
    if (g_drawing_area) {
        SetWidgetState(g_drawing_area, 1);
    }
    
    /* 等待窗口系统稳定 */
    usleep(100000); /* 等待100ms */
    
    /* 更新初始显示 */
    update_status_display();
    
    // 键盘监控功能已移除，直接使用libsx键盘处理
    
    return 0;
}

/* 清理GUI资源 */
void cleanup_gui(void) {
    /* libsx会自动清理资源 */
}

/* 绘制游戏棋盘 */
void draw_board(Widget w, int width, int height, void *data) {
    int i, j;
    int x, y;
    
    /* 避免编译器警告 */
    (void)w;
    (void)data;
    
    /* 检查颜色是否有效 */
    if (color_white == -1 || color_black == -1 || color_blue == -1 || color_yellow == -1) {
        printf("警告: 颜色未正确初始化，使用默认绘制\n");
        /* 绘制一个简单的占位符 */
        DrawFilledBox(0, 0, width, height);
        return;
    }
    
    /* 设置黑色背景 */
    SetColor(color_black);
    DrawFilledBox(0, 0, width, height);
    
    if (!g_game_state) {
        printf("警告: 游戏状态未初始化，绘制空白棋盘\n");
        /* 绘制网格线作为占位符 */
        SetColor(color_black);
        for (i = 0; i <= get_board_height(); i++) {
            int y_line = i * CELL_SIZE;
            if (y_line < height) {
                DrawLine(0, y_line, width, y_line);
            }
        }
        for (j = 0; j <= get_board_width(); j++) {
            int x_line = j * CELL_SIZE;
            if (x_line < width) {
                DrawLine(x_line, 0, x_line, height);
            }
        }
        return;
    }
    
    /* 绘制棋盘 */
    for (i = 0; i < get_board_height(); i++) {
        for (j = 0; j < get_board_width(); j++) {
            x = j * CELL_SIZE;
            y = i * CELL_SIZE;
            
            /* 边界检查 */
            if (x >= width || y >= height) continue;
            
            switch (g_game_state->board[i][j]) {
                case CELL_WALL:
                    /* 绘制粉色墙壁 */
                    SetColor(color_pink);
                    DrawFilledBox(x, y, CELL_SIZE, CELL_SIZE);
                    /* 添加边框 */
                    SetColor(color_blue);
                    DrawBox(x, y, CELL_SIZE, CELL_SIZE);
                    break;
                    
                case CELL_DOT:
                    /* 绘制白色小圆点 */
                    SetColor(color_white);
                    if (CELL_SIZE >= 6) {
                        int dot_size = 3;
                        int center_x = x + CELL_SIZE/2;
                        int center_y = y + CELL_SIZE/2;
                        DrawFilledBox(center_x - dot_size/2, center_y - dot_size/2, dot_size, dot_size);
                    }
                    break;
                    
                case CELL_POWER_DOT:
                    /* 绘制大能量豆 */
                    SetColor(color_white);
                    if (CELL_SIZE >= 8) {
                        int dot_size = 6;
                        int center_x = x + CELL_SIZE/2;
                        int center_y = y + CELL_SIZE/2;
                        DrawFilledBox(center_x - dot_size/2, center_y - dot_size/2, dot_size, dot_size);
                    }
                    break;
                    
                case CELL_PLAYER:
                    /* 绘制黄色PacMan */
                    SetColor(color_yellow);
                    if (CELL_SIZE >= 8) {
                        int pac_size = CELL_SIZE - 6;
                        DrawFilledBox(x + 3, y + 3, pac_size, pac_size);
                        /* 添加黑色边框 */
                        SetColor(color_black);
                        DrawBox(x + 3, y + 3, pac_size, pac_size);
                    }
                    break;
                    
                case CELL_GHOST_RED:
                    /* 绘制红色幽灵 */
                    SetColor(color_red);
                    if (CELL_SIZE >= 8) {
                        int ghost_size = CELL_SIZE - 4;
                        DrawFilledBox(x + 2, y + 2, ghost_size, ghost_size);
                        /* 添加眼睛 */
                        SetColor(color_white);
                        DrawFilledBox(x + 6, y + 6, 4, 4);
                        DrawFilledBox(x + 14, y + 6, 4, 4);
                        SetColor(color_black);
                        DrawFilledBox(x + 7, y + 7, 2, 2);
                        DrawFilledBox(x + 15, y + 7, 2, 2);
                    }
                    break;
                    
                case CELL_GHOST_BLUE:
                    /* 绘制蓝色幽灵 */
                    SetColor(color_cyan);
                    if (CELL_SIZE >= 8) {
                        int ghost_size = CELL_SIZE - 4;
                        DrawFilledBox(x + 2, y + 2, ghost_size, ghost_size);
                        /* 添加眼睛 */
                        SetColor(color_white);
                        DrawFilledBox(x + 6, y + 6, 4, 4);
                        DrawFilledBox(x + 14, y + 6, 4, 4);
                        SetColor(color_black);
                        DrawFilledBox(x + 7, y + 7, 2, 2);
                        DrawFilledBox(x + 15, y + 7, 2, 2);
                    }
                    break;
                    
                case CELL_GHOST_PURPLE:
                    /* 绘制紫色幽灵 */
                    SetColor(color_purple);
                    if (CELL_SIZE >= 8) {
                        int ghost_size = CELL_SIZE - 4;
                        DrawFilledBox(x + 2, y + 2, ghost_size, ghost_size);
                        /* 添加眼睛 */
                        SetColor(color_white);
                        DrawFilledBox(x + 6, y + 6, 4, 4);
                        DrawFilledBox(x + 14, y + 6, 4, 4);
                        SetColor(color_black);
                        DrawFilledBox(x + 7, y + 7, 2, 2);
                        DrawFilledBox(x + 15, y + 7, 2, 2);
                    }
                    break;
                    
                case CELL_GHOST_ORANGE:
                    /* 绘制橙色幽灵 */
                    SetColor(color_orange);
                    if (CELL_SIZE >= 8) {
                        int ghost_size = CELL_SIZE - 4;
                        DrawFilledBox(x + 2, y + 2, ghost_size, ghost_size);
                        /* 添加眼睛 */
                        SetColor(color_white);
                        DrawFilledBox(x + 6, y + 6, 4, 4);
                        DrawFilledBox(x + 14, y + 6, 4, 4);
                        SetColor(color_black);
                        DrawFilledBox(x + 7, y + 7, 2, 2);
                        DrawFilledBox(x + 15, y + 7, 2, 2);
                    }
                    break;
                    
                case CELL_FRUIT:
                    /* 绘制水果奖励 */
                    SetColor(color_green);
                    if (CELL_SIZE >= 8) {
                        int fruit_size = CELL_SIZE - 8;
                        DrawFilledBox(x + 4, y + 4, fruit_size, fruit_size);
                    }
                    break;
                    
                case CELL_EMPTY:
                default:
                    /* 空格显示黑色通道 */
                    SetColor(color_black);
                    DrawFilledBox(x, y, CELL_SIZE, CELL_SIZE);
                    break;
            }
        }
    }
}

/* 算法按钮回调函数 */
void button_random_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (!g_game_state) {
        return;
    }
    
    /* 停止当前自动移动 */
    g_game_state->auto_move_enabled = 0;
    
    /* 启动随机幽灵移动算法 */
    set_algorithm(ALGO_RANDOM);
    
    update_status_display();
}

void button_zigzag_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (!g_game_state) {
        return;
    }
    
    /* 停止当前自动移动 */
    g_game_state->auto_move_enabled = 0;
    
    /* 启动Zig-Zag幽灵移动算法 */
    set_algorithm(ALGO_ZIGZAG);
    
    update_status_display();
}

void button_dfs_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (!g_game_state) {
        return;
    }
    
    /* 停止当前自动移动 */
    g_game_state->auto_move_enabled = 0;
    
    /* 启动追踪幽灵移动算法 */
    set_algorithm(ALGO_DFS);
    
    update_status_display();
}

void button_stop_algo_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (!g_game_state) {
        return;
    }
    
    /* 停止所有幽灵算法和自动移动 */
    g_game_state->auto_move_enabled = 0;
    stop_algorithm();
    
    printf("停止所有幽灵自动算法\n");
    update_status_display();
}

/* 更新显示 */
void update_display(void) {
    /* 处理自动移动 - 在每次更新显示时检查 */
    process_auto_move();
    
    if (g_drawing_area && g_game_state) {
        /* 重新绘制棋盘 */
        draw_board(g_drawing_area, get_board_width() * CELL_SIZE, 
                   get_board_height() * CELL_SIZE, NULL);
    }
    update_status_display();
}

/* 显示胜利消息 */
void show_victory_message(void) {
    printf("\n=== VICTORY! ===\n");
    printf("恭喜！你成功收集了所有豆子！\n");
    printf("总移动次数: %d\n", get_moves_count());
    printf("===============\n\n");
    
    /* 更新状态显示以显示胜利信息 */
    update_status_display();
    
    printf("*** 游戏胜利！请点击主界面的 'Restart' 按钮重新开始游戏\n");
}

/* 更新状态显示 */
void update_status_display(void) {
    char status_text[256];
    
    if (!g_game_state) {
        snprintf(status_text, sizeof(status_text), "Game Status: Not Initialized");
    } else if (is_game_over()) {
        if (is_game_won()) {
            snprintf(status_text, sizeof(status_text), 
                    "*** VICTORY! *** Score: %d | Lives: %d | Click 'Restart' for new game", 
                    g_game_state->score, g_game_state->lives);
        } else {
            snprintf(status_text, sizeof(status_text), 
                    "*** GAME OVER *** Caught by Ghost! Final Score: %d | Moves: %d | Click 'Restart'", 
                    g_game_state->score, g_game_state->moves_count);
        }
    } else {
        /* 检查是否有活跃的算法 */
        if (is_algorithm_enabled()) {
            snprintf(status_text, sizeof(status_text), 
                    "Score: %d | Lives: %d | Dots: %d/%d | Moves: %d | Algorithm: %s", 
                    g_game_state->score,
                    g_game_state->lives,
                    g_game_state->dots_collected,
                    g_game_state->total_dots,
                    get_moves_count(),
                    get_algorithm_name());
        } else {
            snprintf(status_text, sizeof(status_text), 
                    "Score: %d | Lives: %d | Level: %d | Dots: %d/%d | Moves: %d", 
                    g_game_state->score,
                    g_game_state->lives,
                    g_game_state->level,
                    g_game_state->dots_collected,
                    g_game_state->total_dots,
                    get_moves_count());
        }
    }
    
    /* 更新状态标签 */
    if (g_status_label) {
        SetLabel(g_status_label, status_text);
    }
}

/* 移动玩家函数 */
void move_player(Direction dir) {
    /* 检查游戏状态 */
    if (!g_game_state) {
        return;
    }
    
    if (is_game_over()) {
        return;
    }
    
    PlayerPosition current_pos = get_player_position();
    
    int new_x = current_pos.x;
    int new_y = current_pos.y;
    
    switch (dir) {
        case DIR_UP: 
            new_y--; 
            break;
        case DIR_DOWN: 
            new_y++; 
            break;
        case DIR_LEFT: 
            new_x--; 
            break;
        case DIR_RIGHT: 
            new_x++; 
            break;
        default: 
            return;
    }
    
    /* 尝试移动玩家 */
    if (move_player_to(new_x, new_y)) {
        /* 检查胜利条件 */
        if (is_game_won()) {
            show_victory_message();
        }
        
        /* 如果启用了自动移动且移动成功，继续朝同一方向移动 */
        if (g_game_state && g_game_state->auto_move_enabled && 
            g_game_state->auto_move_direction == dir) {
            /* 更新上次移动时间 */
            g_game_state->last_move_time = get_current_time_ms();
        }
    } else {
        /* 移动失败，停止自动移动 */
        if (g_game_state) {
            g_game_state->auto_move_enabled = 0;
        }
    }
    
    update_display();
}

/* 设置自动移动方向 */
void set_auto_move_direction(Direction dir) {
    if (!g_game_state) {
        return;
    }
    
    /* 启用自动移动并设置方向 */
    g_game_state->auto_move_direction = dir;
    g_game_state->auto_move_enabled = 1;
    g_game_state->last_move_time = get_current_time_ms();
    
    /* 立即执行一次移动 */
    move_player(dir);
}

/* 处理自动移动 */
void process_auto_move(void) {
    if (!g_game_state || !g_game_state->auto_move_enabled) {
        return;
    }
    
    if (is_game_over()) {
        return;
    }
    
    /* 检查是否已经过了0.5秒 */
    long current_time = get_current_time_ms();
    if (current_time - g_game_state->last_move_time >= 500) {
        /* 继续朝当前方向移动 */
        move_player(g_game_state->auto_move_direction);
    }
}

/* 按钮回调函数 */
void button_up_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    set_auto_move_direction(DIR_UP);
}

void button_down_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    set_auto_move_direction(DIR_DOWN);
}

void button_left_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    set_auto_move_direction(DIR_LEFT);
}

void button_right_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    set_auto_move_direction(DIR_RIGHT);
}

void button_rejouer_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (!g_game_state) {
        return;
    }
    
    /* 使用重置函数 */
    reset_game_state();
    
    /* 更新显示 */
    update_display();
}

void button_aide_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    printf("\n=== PacMan 游戏帮助 ===\n");
    printf("游戏目标: 收集所有蓝色圆点\n");
    printf("控制方式: WASD键或方向键移动\n");
    printf("其他操作: R键重新开始，Q键退出\n");
    printf("======================\n");
}

/* 提供给X11键盘包装器的接口函数 */
void show_help() {
    button_aide_callback(NULL, NULL);
}

void quit_game() {
    button_quit_callback(NULL, NULL);
}

void button_quit_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    exit(0);
}

/* 键盘事件处理 */
void key_press_callback(Widget w, char *input, int up_or_down, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (up_or_down == 0) { /* 按键按下 */
        /* 检查方向键（特殊键码） */
        if (input[0] == 27 && input[1] == '[') { /* ESC序列，可能是方向键 */
            switch (input[2]) {
                case 'A': /* 上方向键 */
                    set_auto_move_direction(DIR_UP);
                    return;
                case 'B': /* 下方向键 */
                    set_auto_move_direction(DIR_DOWN);
                    return;
                case 'C': /* 右方向键 */
                    set_auto_move_direction(DIR_RIGHT);
                    return;
                case 'D': /* 左方向键 */
                    set_auto_move_direction(DIR_LEFT);
                    return;
            }
        }
        
        /* 处理普通按键 */
        switch (input[0]) {
            case 'w': case 'W':
                set_auto_move_direction(DIR_UP);
                break;
            case 's': case 'S':
                set_auto_move_direction(DIR_DOWN);
                break;
            case 'a': case 'A':
                set_auto_move_direction(DIR_LEFT);
                break;
            case 'd': case 'D':
                set_auto_move_direction(DIR_RIGHT);
                break;
            case 'r': case 'R':
                button_rejouer_callback(w, data);
                break;
            case 'h': case 'H':
                button_aide_callback(w, data);
                break;
            case 'q': case 'Q':
                button_quit_callback(w, data);
                break;
            /* 数字键盘方向键支持 */
            case '8': /* 数字键盘8 - 上 */
                set_auto_move_direction(DIR_UP);
                break;
            case '2': /* 数字键盘2 - 下 */
                set_auto_move_direction(DIR_DOWN);
                break;
            case '4': /* 数字键盘4 - 左 */
                set_auto_move_direction(DIR_LEFT);
                break;
            case '6': /* 数字键盘6 - 右 */
                set_auto_move_direction(DIR_RIGHT);
                break;
        }
    }
}