#ifndef GAME_H
#define GAME_H

#include "types.h"

/* 游戏初始化和清理函数 */
int init_game_state(void);
void cleanup_game_state(void);
void reset_game_state(void);

/* 棋盘管理函数 */
void init_board(void);
void generate_random_dots(int num_dots);
void clear_board_cell(int x, int y);
CellType get_board_cell(int x, int y);
void set_board_cell(int x, int y, CellType type);

/* 玩家移动和位置管理 */
int is_valid_move(int x, int y);
int move_player_to(int new_x, int new_y);
void update_player_position(int x, int y);
PlayerPosition get_player_position(void);

/* 碰撞检测和边界处理 */
int is_within_bounds(int x, int y);
int is_wall_collision(int x, int y);
int check_dot_collection(int x, int y);

/* 游戏状态查询 */
int get_dots_collected(void);
int get_total_dots(void);
int get_remaining_dots(void);
int get_moves_count(void);
int is_game_over(void);
int is_game_won(void);

/* 游戏逻辑更新 */
void increment_moves(void);
void collect_dot(void);
void check_win_condition(void);
void update_game_statistics(void);



/* 全局游戏状态访问 */
extern GameState *g_game_state;

#endif /* GAME_H */