#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "gui.h"

/* 全局游戏状态 */
GameState *g_game_state = NULL;

/* 初始化游戏状态 */
int init_game_state(void) {
    int i, j;
    
    g_game_state = (GameState*)malloc(sizeof(GameState));
    if (!g_game_state) {
        fprintf(stderr, "错误: 无法分配游戏状态内存\n");
        return -1;
    }
    
    /* 初始化棋盘 */
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
    
    /* 设置玩家初始位置 */
    g_game_state->player_pos.x = 1;
    g_game_state->player_pos.y = 1;
    g_game_state->board[1][1] = CELL_PLAYER;
    
    /* 初始化游戏统计 */
    g_game_state->dots_collected = 0;
    g_game_state->total_dots = (BOARD_WIDTH-2) * (BOARD_HEIGHT-2) - 1; /* 减去玩家位置 */
    g_game_state->moves_count = 0;
    g_game_state->game_over = 0;
    g_game_state->game_won = 0;
    
    return 0;
}

/* 清理游戏状态 */
void cleanup_game_state(void) {
    if (g_game_state) {
        free(g_game_state);
        g_game_state = NULL;
    }
}

/* 打印使用说明 */
void print_usage(const char *program_name) {
    printf("使用方法: %s [选项]\n", program_name);
    printf("选项:\n");
    printf("  -h, --help    显示此帮助信息\n");
    printf("  -v, --version 显示版本信息\n");
    printf("\n");
    printf("游戏控制:\n");
    printf("  方向键或按钮: 移动玩家\n");
    printf("  Rejouer: 重新开始游戏\n");
    printf("  Aide: 显示帮助信息\n");
    printf("  Quit: 退出游戏\n");
}

/* 主函数 */
int main(int argc, char *argv[]) {
    int i;
    
    /* 处理命令行参数 */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("PacMan 游戏 v1.0\n");
            printf("使用C语言和libsx图形库开发\n");
            return 0;
        } else {
            fprintf(stderr, "未知参数: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    /* 先初始化游戏状态 */
    printf("正在初始化游戏状态...\n");
    if (init_game_state() != 0) {
        fprintf(stderr, "游戏状态初始化失败\n");
        return 1;
    }
    printf("游戏状态初始化完成\n");
    
    /* 然后初始化GUI */
    printf("正在初始化GUI...\n");
    if (init_gui(argc, argv) != 0) {
        fprintf(stderr, "GUI初始化失败\n");
        cleanup_game_state();
        return 1;
    }
    printf("GUI初始化完成\n");
    
    /* 进入主循环 */
    MainLoop();
    
    /* 清理资源 */
    cleanup_gui();
    cleanup_game_state();
    
    return 0;
}