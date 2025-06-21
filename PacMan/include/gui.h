#ifndef GUI_H
#define GUI_H

#include "types.h"
#include <libsx.h>

/* GUI组件ID定义 */
#define MAIN_WINDOW 1
#define DRAWING_AREA 2
#define UP_BUTTON 3
#define DOWN_BUTTON 4
#define LEFT_BUTTON 5
#define RIGHT_BUTTON 6
#define REJOUER_BUTTON 7
#define AIDE_BUTTON 8
#define QUIT_BUTTON 9
#define STATUS_LABEL 10
#define RANDOM_BUTTON 11
#define ZIGZAG_BUTTON 12
#define DFS_BUTTON 13
#define STOP_ALGO_BUTTON 14

/* GUI初始化和销毁函数 */
int init_gui(int argc, char *argv[]);
void cleanup_gui(void);

/* 绘制函数 */
void draw_board(Widget w, int width, int height, void *data);
void update_display(void);
void update_status_display(void);
void show_victory_message(void);

/* 游戏逻辑函数 */
void move_player(Direction dir);
void set_auto_move_direction(Direction dir);
void process_auto_move(void);

/* 按钮回调函数 */
void button_up_callback(Widget w, void *data);
void button_down_callback(Widget w, void *data);
void button_left_callback(Widget w, void *data);
void button_right_callback(Widget w, void *data);
void button_rejouer_callback(Widget w, void *data);
void button_aide_callback(Widget w, void *data);
void button_quit_callback(Widget w, void *data);

/* 算法按钮回调函数 */
void button_random_callback(Widget w, void *data);
void button_zigzag_callback(Widget w, void *data);
void button_dfs_callback(Widget w, void *data);
void button_stop_algo_callback(Widget w, void *data);

/* 胜利提示框回调函数 */


/* 键盘事件处理 */
void key_press_callback(Widget w, char *input, int up_or_down, void *data);

/* 全局变量声明 */
extern GameState *g_game_state;
extern Widget g_main_window;
extern Widget g_drawing_area;
extern Widget g_status_label;

#endif /* GUI_H */