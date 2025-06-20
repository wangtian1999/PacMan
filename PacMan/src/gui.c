#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"
#include "types.h"

/* 全局GUI组件 */
Widget g_main_window;
Widget g_drawing_area;
Widget g_status_label;

/* 颜色定义 */
static int color_black, color_white, color_blue, color_yellow, color_red;

/* 初始化GUI */
int init_gui(int argc, char *argv[]) {
    Widget button_up, button_down, button_left, button_right;
    Widget button_rejouer, button_aide, button_quit;
    
    /* 初始化libsx */
    printf("正在初始化libsx显示系统...\n");
    if (OpenDisplay(argc, argv) == 0) {
        fprintf(stderr, "错误: 无法打开显示\n");
        return -1;
    }
    printf("libsx显示系统初始化成功\n");
    
    /* 创建主窗口 */
    printf("正在创建主窗口...\n");
    g_main_window = MakeWindow("PacMan Game", NULL, 0);
    if (!g_main_window) {
        fprintf(stderr, "错误: 无法创建主窗口\n");
        return -1;
    }
    printf("主窗口创建成功\n");
    
    /* 初始化颜色 */
    printf("正在初始化颜色...\n");
    color_black = GetNamedColor("black");
    color_white = GetNamedColor("white");
    color_blue = GetNamedColor("blue");
    color_yellow = GetNamedColor("yellow");
    color_red = GetNamedColor("red");
    printf("颜色初始化完成: white=%d, black=%d, blue=%d, yellow=%d\n", 
           color_white, color_black, color_blue, color_yellow);
    
    /* 检查颜色是否成功初始化 */
    if (color_black == -1 || color_white == -1 || color_blue == -1 || 
        color_yellow == -1 || color_red == -1) {
        fprintf(stderr, "警告: 某些颜色初始化失败\n");
    }
    
    /* 创建绘图区域 */
    printf("正在创建绘图区域 (%dx%d)...\n", BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE);
    g_drawing_area = MakeDrawArea(BOARD_WIDTH * CELL_SIZE, 
                                  BOARD_HEIGHT * CELL_SIZE, 
                                  draw_board, NULL);
    if (!g_drawing_area) {
        fprintf(stderr, "错误: 无法创建绘图区域\n");
        return -1;
    }
    printf("绘图区域创建成功\n");
    
    /* 创建方向控制按钮 */
    button_up = MakeButton("Up", button_up_callback, NULL);
    button_down = MakeButton("Down", button_down_callback, NULL);
    button_left = MakeButton("Left", button_left_callback, NULL);
    button_right = MakeButton("Right", button_right_callback, NULL);
    
    /* 创建功能按钮 */
    button_rejouer = MakeButton("Restart", button_rejouer_callback, NULL);
    button_aide = MakeButton("Help", button_aide_callback, NULL);
    button_quit = MakeButton("Quit", button_quit_callback, NULL);
    
    /* 创建状态显示标签 */
    g_status_label = MakeLabel("Moves: 0  Collected: 0  Remaining: 0");
    
    /* 设置布局 - 添加错误检查 */
    printf("开始设置GUI布局...\n");
    
    /* 检查所有widget是否有效 */
    if (!g_drawing_area || !button_up || !button_left || !button_down || !button_right ||
        !button_rejouer || !button_aide || !button_quit || !g_status_label) {
        fprintf(stderr, "错误: 某些GUI组件创建失败\n");
        return -1;
    }
    
    /* 绘图区域布局 */
    printf("设置绘图区域布局...\n");
    /* 绘图区域不需要相对于主窗口设置位置，它已经是主窗口的子组件 */
    printf("绘图区域布局设置完成（无需额外设置）\n");
    
    /* 设置方向按钮布局 */
    printf("设置方向按钮布局...\n");
    if (button_up && g_drawing_area) {
        SetWidgetPos(button_up, PLACE_UNDER, g_drawing_area, NO_CARE, NULL);
        printf("上按钮布局设置完成\n");
    }
    if (button_left && button_up) {
        SetWidgetPos(button_left, PLACE_UNDER, button_up, NO_CARE, NULL);
        printf("左按钮布局设置完成\n");
    }
    if (button_down && button_left) {
        SetWidgetPos(button_down, PLACE_RIGHT, button_left, NO_CARE, NULL);
        printf("下按钮布局设置完成\n");
    }
    if (button_right && button_down) {
        SetWidgetPos(button_right, PLACE_RIGHT, button_down, NO_CARE, NULL);
        printf("右按钮布局设置完成\n");
    }
    
    /* 功能按钮布局 */
    printf("设置功能按钮布局...\n");
    if (button_rejouer && button_left) {
        SetWidgetPos(button_rejouer, PLACE_UNDER, button_left, NO_CARE, NULL);
        printf("重新开始按钮布局设置完成\n");
    }
    if (button_aide && button_rejouer) {
        SetWidgetPos(button_aide, PLACE_RIGHT, button_rejouer, NO_CARE, NULL);
        printf("帮助按钮布局设置完成\n");
    }
    if (button_quit && button_aide) {
        SetWidgetPos(button_quit, PLACE_RIGHT, button_aide, NO_CARE, NULL);
        printf("退出按钮布局设置完成\n");
    }
    
    /* 状态标签布局 */
    printf("设置状态标签布局...\n");
    if (g_status_label && button_rejouer) {
        SetWidgetPos(g_status_label, PLACE_UNDER, button_rejouer, NO_CARE, NULL);
        printf("状态标签布局设置完成\n");
    }
    
    printf("GUI布局设置完成\n");
    
    /* 设置键盘事件处理 */
    SetKeypressCB(g_main_window, key_press_callback);
    
    /* 显示窗口 */
    ShowDisplay();
    
    /* 更新初始显示 */
    update_status_display();
    
    /* 绘图回调已在创建时设置 */
    
    printf("GUI初始化完成\n");
    
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
    
    /* 清空绘图区域 */
    SetColor(color_white);
    DrawFilledBox(0, 0, width, height);
    
    if (!g_game_state) {
        printf("警告: 游戏状态未初始化，绘制空白棋盘\n");
        /* 绘制网格线作为占位符 */
        SetColor(color_black);
        for (i = 0; i <= BOARD_HEIGHT; i++) {
            int y_line = i * CELL_SIZE;
            if (y_line < height) {
                DrawLine(0, y_line, width, y_line);
            }
        }
        for (j = 0; j <= BOARD_WIDTH; j++) {
            int x_line = j * CELL_SIZE;
            if (x_line < width) {
                DrawLine(x_line, 0, x_line, height);
            }
        }
        return;
    }
    
    /* 绘制棋盘 */
    for (i = 0; i < BOARD_HEIGHT && i < 15; i++) {
        for (j = 0; j < BOARD_WIDTH && j < 20; j++) {
            x = j * CELL_SIZE;
            y = i * CELL_SIZE;
            
            /* 边界检查 */
            if (x >= width || y >= height) continue;
            
            switch (g_game_state->board[i][j]) {
                case CELL_WALL:
                    SetColor(color_black);
                    DrawFilledBox(x, y, CELL_SIZE, CELL_SIZE);
                    break;
                    
                case CELL_DOT:
                    SetColor(color_blue);
                    if (CELL_SIZE >= 8) {
                        DrawFilledBox(x + CELL_SIZE/2 - 2, y + CELL_SIZE/2 - 2, 4, 4);
                    }
                    break;
                    
                case CELL_PLAYER:
                    SetColor(color_yellow);
                    if (CELL_SIZE >= 6) {
                        DrawFilledBox(x + 2, y + 2, CELL_SIZE - 4, CELL_SIZE - 4);
                    }
                    break;
                    
                case CELL_EMPTY:
                default:
                    /* 空格不需要绘制 */
                    break;
            }
        }
    }
}

/* 更新显示 */
void update_display(void) {
    if (g_drawing_area && g_game_state) {
        /* 重新绘制棋盘 */
        draw_board(g_drawing_area, BOARD_WIDTH * CELL_SIZE, 
                   BOARD_HEIGHT * CELL_SIZE, NULL);
    }
    update_status_display();
}

/* 更新状态显示 */
void update_status_display(void) {
    char status_text[256];
    
    if (!g_game_state) {
        snprintf(status_text, sizeof(status_text), "Game Status: Not Initialized");
    } else if (g_game_state->game_over) {
        if (g_game_state->game_won) {
            snprintf(status_text, sizeof(status_text), 
                    "Congratulations! You Won in %d moves! Press R to restart", 
                    g_game_state->moves_count);
        } else {
            snprintf(status_text, sizeof(status_text), "Game Over - Press R to restart");
        }
    } else {
        int remaining_dots = g_game_state->total_dots - g_game_state->dots_collected;
        snprintf(status_text, sizeof(status_text), 
                "Remaining: %d | Moves: %d | Use WASD or buttons to move", 
                remaining_dots, 
                g_game_state->moves_count);
    }
    
    /* 更新状态标签 */
    if (g_status_label) {
        SetLabel(g_status_label, status_text);
    }
}

/* 移动玩家函数 */
void move_player(Direction dir) {
    if (!g_game_state || g_game_state->game_over) return;
    
    int new_x = g_game_state->player_pos.x;
    int new_y = g_game_state->player_pos.y;
    
    switch (dir) {
        case DIR_UP: new_y--; break;
        case DIR_DOWN: new_y++; break;
        case DIR_LEFT: new_x--; break;
        case DIR_RIGHT: new_x++; break;
        default: return;
    }
    
    /* 边界检查 */
    if (new_x < 0 || new_x >= BOARD_WIDTH || 
        new_y < 0 || new_y >= BOARD_HEIGHT) {
        printf("移动超出边界\n");
        return;
    }
    
    /* 墙壁检查 */
    if (g_game_state->board[new_y][new_x] == CELL_WALL) {
        printf("撞墙了！\n");
        return;
    }
    
    /* 收集豆子 */
    if (g_game_state->board[new_y][new_x] == CELL_DOT) {
        g_game_state->dots_collected++;
        printf("收集了一个豆子！剩余: %d\n", 
               g_game_state->total_dots - g_game_state->dots_collected);
    }
    
    /* 更新位置 */
    g_game_state->board[g_game_state->player_pos.y][g_game_state->player_pos.x] = CELL_EMPTY;
    g_game_state->player_pos.x = new_x;
    g_game_state->player_pos.y = new_y;
    g_game_state->board[new_y][new_x] = CELL_PLAYER;
    g_game_state->moves_count++;
    
    /* 检查胜利条件 */
    if (g_game_state->dots_collected >= g_game_state->total_dots) {
        g_game_state->game_won = 1;
        g_game_state->game_over = 1;
        printf("恭喜！您赢了！用了 %d 步完成游戏！\n", g_game_state->moves_count);
    }
    
    update_display();
}

/* 按钮回调函数 */
void button_up_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("上移\n");
    move_player(DIR_UP);
}

void button_down_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("下移\n");
    move_player(DIR_DOWN);
}

void button_left_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("左移\n");
    move_player(DIR_LEFT);
}

void button_right_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("右移\n");
    move_player(DIR_RIGHT);
}

void button_rejouer_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("重新开始游戏\n");
    
    if (!g_game_state) return;
    
    /* 重新初始化游戏状态 */
    int i, j;
    
    /* 重新初始化棋盘 */
    for (i = 0; i < BOARD_HEIGHT; i++) {
        for (j = 0; j < BOARD_WIDTH; j++) {
            /* 边界设为墙 */
            if (i == 0 || i == BOARD_HEIGHT-1 || j == 0 || j == BOARD_WIDTH-1) {
                g_game_state->board[i][j] = CELL_WALL;
            } else {
                g_game_state->board[i][j] = CELL_DOT;
            }
        }
    }
    
    /* 重置玩家位置 */
    g_game_state->player_pos.x = 1;
    g_game_state->player_pos.y = 1;
    g_game_state->board[1][1] = CELL_PLAYER;
    
    /* 重置游戏统计 */
    g_game_state->dots_collected = 0;
    g_game_state->total_dots = (BOARD_WIDTH-2) * (BOARD_HEIGHT-2) - 1;
    g_game_state->moves_count = 0;
    g_game_state->game_over = 0;
    g_game_state->game_won = 0;
    
    printf("游戏已重新开始！\n");
    update_display();
}

void button_aide_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("帮助按钮被点击\n");
    printf("游戏说明:\n");
    printf("- 使用方向键或按钮移动玩家\n");
    printf("- 收集所有蓝色圆点获胜\n");
    printf("- 黑色方块是墙壁，无法通过\n");
}

void button_quit_callback(Widget w, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    printf("退出按钮被点击\n");
    exit(0);
}

/* 键盘事件处理 */
void key_press_callback(Widget w, char *input, int up_or_down, void *data) {
    (void)w; (void)data; /* 避免未使用参数警告 */
    
    if (up_or_down == 0) { /* 按键按下 */
        printf("按键被按下: %c\n", input[0]);
        
        switch (input[0]) {
            case 'w': case 'W':
                printf("键盘上移\n");
                move_player(DIR_UP);
                break;
            case 's': case 'S':
                printf("键盘下移\n");
                move_player(DIR_DOWN);
                break;
            case 'a': case 'A':
                printf("键盘左移\n");
                move_player(DIR_LEFT);
                break;
            case 'd': case 'D':
                printf("键盘右移\n");
                move_player(DIR_RIGHT);
                break;
            case 'r': case 'R':
                printf("键盘重新开始\n");
                button_rejouer_callback(w, data);
                break;
            case 'h': case 'H':
                button_aide_callback(w, data);
                break;
            case 'q': case 'Q':
                printf("键盘退出游戏\n");
                button_quit_callback(w, data);
                break;
            default:
                printf("未知按键: %c (使用WASD移动，R重新开始，Q退出)\n", input[0]);
                break;
        }
    }
}