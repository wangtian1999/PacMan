#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "gui.h"
#include "game.h"

/* 打印使用说明 */
void print_usage(const char *program_name) {
    printf("使用方法: %s [选项] [宽度] [高度]\n", program_name);
    printf("选项:\n");
    printf("  -h, --help    显示此帮助信息\n");
    printf("  -v, --version 显示版本信息\n");
    printf("  -s, --size    指定网格大小 (格式: -s 宽度 高度)\n");
    printf("\n");
    printf("网格大小:\n");
    printf("  宽度范围: %d - %d (默认: %d)\n", MIN_BOARD_WIDTH, MAX_BOARD_WIDTH, DEFAULT_BOARD_WIDTH);
    printf("  高度范围: %d - %d (默认: %d)\n", MIN_BOARD_HEIGHT, MAX_BOARD_HEIGHT, DEFAULT_BOARD_HEIGHT);
    printf("  示例: %s -s 25 20\n", program_name);
    printf("  示例: %s 30 25\n", program_name);
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
    int board_width = DEFAULT_BOARD_WIDTH;
    int board_height = DEFAULT_BOARD_HEIGHT;
    int size_specified = 0;
    
    /* 处理命令行参数 */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            printf("PacMan 游戏 v1.0\n");
            printf("使用C语言和libsx图形库开发\n");
            return 0;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) {
            /* 检查是否有足够的参数 */
            if (i + 2 >= argc) {
                fprintf(stderr, "错误: -s 选项需要两个参数 (宽度 高度)\n");
                print_usage(argv[0]);
                return 1;
            }
            board_width = atoi(argv[++i]);
            board_height = atoi(argv[++i]);
            size_specified = 1;
        } else if (argv[i][0] != '-' && !size_specified) {
            /* 直接指定宽度和高度 */
            if (i + 1 < argc && argv[i+1][0] != '-') {
                board_width = atoi(argv[i]);
                board_height = atoi(argv[i+1]);
                size_specified = 1;
                i++; /* 跳过下一个参数 */
            } else {
                fprintf(stderr, "错误: 需要同时指定宽度和高度\n");
                print_usage(argv[0]);
                return 1;
            }
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "未知参数: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    /* 验证网格大小 */
    if (board_width < MIN_BOARD_WIDTH || board_width > MAX_BOARD_WIDTH) {
        fprintf(stderr, "错误: 宽度必须在 %d 到 %d 之间\n", MIN_BOARD_WIDTH, MAX_BOARD_WIDTH);
        return 1;
    }
    if (board_height < MIN_BOARD_HEIGHT || board_height > MAX_BOARD_HEIGHT) {
        fprintf(stderr, "错误: 高度必须在 %d 到 %d 之间\n", MIN_BOARD_HEIGHT, MAX_BOARD_HEIGHT);
        return 1;
    }
    
    /* 设置网格大小 */
    set_board_size(board_width, board_height);
    printf("游戏网格大小: %d x %d\n", board_width, board_height);
    
    /* 先初始化游戏状态 */
    if (init_game_state_with_size(board_width, board_height) != 0) {
        fprintf(stderr, "游戏状态初始化失败\n");
        return 1;
    }
    
    /* 然后初始化GUI */
    if (init_gui(argc, argv) != 0) {
        fprintf(stderr, "GUI初始化失败\n");
        cleanup_game_state();
        return 1;
    }
    
    /* 进入主循环 */
    MainLoop();
    
    /* 清理资源 */
    cleanup_gui();
    cleanup_game_state();
    
    return 0;
}