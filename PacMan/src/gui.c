#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gui.h"
#include "types.h"
#include "game.h"

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
    printf("正在创建方向控制按钮...\n");
    button_up = MakeButton("Up", button_up_callback, NULL);
    button_down = MakeButton("Down", button_down_callback, NULL);
    button_left = MakeButton("Left", button_left_callback, NULL);
    button_right = MakeButton("Right", button_right_callback, NULL);
    
    if (!button_up || !button_down || !button_left || !button_right) {
        fprintf(stderr, "错误: 方向按钮创建失败\n");
        return -1;
    }
    printf("方向控制按钮创建成功\n");
    
    /* 创建功能按钮 */
    printf("正在创建功能按钮...\n");
    button_rejouer = MakeButton("Restart", button_rejouer_callback, NULL);
    button_aide = MakeButton("Help", button_aide_callback, NULL);
    button_quit = MakeButton("Quit", button_quit_callback, NULL);
    
    if (!button_rejouer || !button_aide || !button_quit) {
        fprintf(stderr, "错误: 功能按钮创建失败\n");
        return -1;
    }
    printf("功能按钮创建成功\n");
    
    /* 创建状态显示标签 */
    printf("正在创建状态标签...\n");
    g_status_label = MakeLabel("Moves: 0  Collected: 0  Remaining: 0");
    
    if (!g_status_label) {
        fprintf(stderr, "错误: 状态标签创建失败\n");
        return -1;
    }
    printf("状态标签创建成功\n");
    
    /* 设置布局 - 添加错误检查 */
    printf("开始设置GUI布局...\n");
    
    /* 检查所有widget是否有效 */
    if (!g_drawing_area || !button_up || !button_left || !button_down || !button_right ||
        !button_rejouer || !button_aide || !button_quit || !g_status_label) {
        fprintf(stderr, "错误: 某些GUI组件创建失败\n");
        return -1;
    }
    
    /* 绘图区域布局 - libsx中绘图区域自动布局 */
    printf("设置绘图区域布局...\n");
    /* 绘图区域不需要相对于主窗口设置位置，它会自动成为主窗口的第一个子组件 */
    printf("绘图区域布局设置完成（自动布局）\n");
    
    /* 设置方向按钮布局 - 独立布局避免链式依赖 */
    printf("设置方向按钮布局...\n");
    
    /* 所有方向按钮都直接相对于绘图区域布局 */
    if (button_left && g_drawing_area) {
        SetWidgetPos(button_left, PLACE_UNDER, g_drawing_area, NO_CARE, NULL);
        printf("左按钮布局设置完成\n");
    }
    
    if (button_down && g_drawing_area) {
        SetWidgetPos(button_down, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_left);
        printf("下按钮布局设置完成\n");
    }
    
    if (button_up && g_drawing_area) {
        SetWidgetPos(button_up, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_down);
        printf("上按钮布局设置完成\n");
    }
    
    if (button_right && g_drawing_area) {
        SetWidgetPos(button_right, PLACE_UNDER, g_drawing_area, PLACE_RIGHT, button_up);
        printf("右按钮布局设置完成\n");
    }
    
    /* 功能按钮布局 - 独立布局避免链式依赖 */
    printf("设置功能按钮布局...\n");
    
    /* 重新开始按钮在左按钮下方 */
    if (button_rejouer && button_left) {
        SetWidgetPos(button_rejouer, PLACE_UNDER, button_left, NO_CARE, NULL);
        printf("重新开始按钮布局设置完成\n");
    }
    
    /* 帮助按钮在左按钮下方，重新开始按钮右侧 */
    if (button_aide && button_left) {
        SetWidgetPos(button_aide, PLACE_UNDER, button_left, PLACE_RIGHT, button_rejouer);
        printf("帮助按钮布局设置完成\n");
    }
    
    /* 退出按钮在左按钮下方，帮助按钮右侧 */
    if (button_quit && button_left) {
        SetWidgetPos(button_quit, PLACE_UNDER, button_left, PLACE_RIGHT, button_aide);
        printf("退出按钮布局设置完成\n");
    }
    
    /* 状态标签布局 - 在功能按钮下方 */
    printf("设置状态标签布局...\n");
    if (g_status_label && button_rejouer) {
        SetWidgetPos(g_status_label, PLACE_UNDER, button_rejouer, NO_CARE, NULL);
        printf("状态标签布局设置完成\n");
    }
    
    printf("GUI布局设置完成\n");
    
    /* 设置键盘事件处理 */
    printf("设置键盘事件处理...\n");
    SetKeypressCB(g_main_window, key_press_callback);
    printf("键盘事件处理设置完成\n");
    
    /* 获取标准颜色 - libsx要求在ShowDisplay之前调用 */
    printf("获取标准颜色...\n");
    GetStandardColors();
    printf("标准颜色获取完成\n");
    
    /* 显示窗口 */
    printf("显示窗口...\n");
    ShowDisplay();
    printf("窗口显示完成\n");
    
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
    } else if (is_game_over()) {
        if (is_game_won()) {
            snprintf(status_text, sizeof(status_text), 
                    "Congratulations! You Won in %d moves! Press R to restart", 
                    get_moves_count());
        } else {
            snprintf(status_text, sizeof(status_text), "Game Over - Press R to restart");
        }
    } else {
        snprintf(status_text, sizeof(status_text), 
                "Remaining: %d | Moves: %d | Use WASD or buttons to move", 
                get_remaining_dots(), 
                get_moves_count());
    }
    
    /* 更新状态标签 */
    if (g_status_label) {
        SetLabel(g_status_label, status_text);
    }
}

/* 移动玩家函数 */
void move_player(Direction dir) {
    if (!g_game_state || is_game_over()) return;
    
    PlayerPosition current_pos = get_player_position();
    int new_x = current_pos.x;
    int new_y = current_pos.y;
    
    switch (dir) {
        case DIR_UP: new_y--; break;
        case DIR_DOWN: new_y++; break;
        case DIR_LEFT: new_x--; break;
        case DIR_RIGHT: new_x++; break;
        default: return;
    }
    
    /* 尝试移动玩家 */
    if (move_player_to(new_x, new_y)) {
        /* 移动成功，输出调试信息 */
        if (check_dot_collection(new_x, new_y)) {
            printf("Collected a dot! Remaining: %d\n", get_remaining_dots());
        }
        
        /* 检查胜利条件 */
        if (is_game_won()) {
            printf("Congratulations! You won in %d moves!\n", get_moves_count());
        }
    } else {
        /* 移动失败，输出调试信息 */
        if (!is_within_bounds(new_x, new_y)) {
            printf("Move out of bounds\n");
        } else if (is_wall_collision(new_x, new_y)) {
            printf("Hit a wall!\n");
        }
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
    printf("Restarting game\n");
    
    if (!g_game_state) return;
    
    /* 使用新的重置函数 */
    reset_game_state();
    
    printf("Game restarted!\n");
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