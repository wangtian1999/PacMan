# PacMan项目Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lsx -lX11 -lXt
INCLUDE = -I/usr/local/include -I./include
LIBPATH = -L/usr/local/lib

# 源文件和目标文件
SRCDIR = src
OBJDIR = obj
# 原始版本只编译main.c和gui.c
SOURCES = $(SRCDIR)/main.c $(SRCDIR)/gui.c $(SRCDIR)/game.c
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
# 可执行文件目标
TARGET = pacman

# 默认目标
all: $(TARGET)

# 创建目标目录
$(OBJDIR):
	mkdir -p $(OBJDIR)

# 编译目标文件
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

# 链接生成可执行文件
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBPATH) $(LIBS) -o $(TARGET)
	@echo "编译完成！可执行文件: $(TARGET)"

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 测试编译环境
test:
	@echo "测试编译环境..."
	@echo "#include <stdio.h>" > test.c
	@echo "#include <libsx.h>" >> test.c
	@echo "int main() { printf(\"libsx环境配置成功!\\n\"); return 0; }" >> test.c
	$(CC) $(CFLAGS) $(INCLUDE) $(LIBPATH) test.c -o test $(LIBS)
	@echo "编译成功！运行测试程序:"
	./test
	@rm -f test test.c
	@echo "环境测试完成！"



# 清理生成的文件
clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf $(OBJDIR)

# 显示帮助信息
help:
	@echo "可用的目标:"
	@echo "  all              - 编译主程序"
	@echo "  run              - 编译并运行主程序"
	@echo "  pacman_safe      - 编译安全版本主程序"
	@echo "  run_safe         - 编译并运行安全版本"
	@echo "  pacman_optimized - 编译优化版本主程序 (推荐)"
	@echo "  run_optimized    - 编译并运行优化版本 (推荐)"
	@echo "  test             - 测试编译环境"
	@echo "  test_simple      - 编译简单测试程序"
	@echo "  test_minimal     - 编译最小测试程序"
	@echo "  clean            - 清理生成的文件"
	@echo "  help             - 显示此帮助信息"
	@echo ""
	@echo "推荐使用: make run_optimized"

.PHONY: all run test clean help test-run test-minimal pacman_safe run_safe pacman_optimized run_optimized
