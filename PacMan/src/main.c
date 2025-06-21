#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "gui.h"
#include "game.h"

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
    if (init_game_state() != 0) {
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