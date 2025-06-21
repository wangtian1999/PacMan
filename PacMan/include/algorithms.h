#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "types.h"

/* 算法类型常量 */
#define ALGO_NONE 0
#define ALGO_RANDOM 1
#define ALGO_ZIGZAG 2
#define ALGO_DFS 3

/* 算法控制函数 */
void set_algorithm(int algorithm_type);
void stop_algorithm(void);
int is_algorithm_enabled(void);

/* 幽灵移动函数 */
void update_ghost_movement(void);

/* 算法信息函数 */
const char* get_algorithm_name(void);

/* 幽灵移动间隔控制 */
void set_ghost_move_interval(int interval_ms);
int get_ghost_move_interval(void);

#endif /* ALGORITHMS_H */