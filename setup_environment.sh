#!/bin/bash

# 吃豆人游戏开发环境自动搭建脚本
# 适用于Ubuntu系统

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印函数
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查是否为Ubuntu系统
check_ubuntu() {
    print_info "检查操作系统..."
    if [[ ! -f /etc/lsb-release ]] || ! grep -q "Ubuntu" /etc/lsb-release; then
        print_error "此脚本仅适用于Ubuntu系统"
        exit 1
    fi
    print_success "检测到Ubuntu系统"
}

# 检查网络连接
check_network() {
    print_info "检查网络连接..."
    if ! ping -c 1 google.com &> /dev/null && ! ping -c 1 baidu.com &> /dev/null; then
        print_error "网络连接失败，请检查网络设置"
        exit 1
    fi
    print_success "网络连接正常"
}

# 更新包管理器
update_packages() {
    print_info "更新包管理器..."
    if sudo apt update; then
        print_success "包管理器更新完成"
    else
        print_error "包管理器更新失败"
        exit 1
    fi
}

# 安装基础开发工具
install_build_tools() {
    print_info "安装基础开发工具..."
    local packages=("build-essential" "gcc" "make" "wget" "tar")
    
    for package in "${packages[@]}"; do
        print_info "安装 $package..."
        if sudo apt install -y "$package"; then
            print_success "$package 安装成功"
        else
            print_error "$package 安装失败"
            exit 1
        fi
    done
}

# 安装X11开发库
install_x11_libs() {
    print_info "安装X11开发库..."
    local x11_packages=("libx11-dev" "libxt-dev" "libxaw7-dev")
    
    for package in "${x11_packages[@]}"; do
        print_info "安装 $package..."
        if sudo apt install -y "$package"; then
            print_success "$package 安装成功"
        else
            print_error "$package 安装失败"
            exit 1
        fi
    done
}

# 下载并安装libsx库
install_libsx() {
    print_info "开始安装libsx库..."
    
    # 使用apt-get安装libsx-dev
    print_info "安装libsx-dev包..."
    if sudo apt-get install -y libsx-dev; then
        print_success "libsx库安装成功"
    else
        print_error "libsx库安装失败"
        exit 1
    fi
}


# 创建项目目录
create_project_dir() {
    print_info "创建项目目录..."
    local project_dir="./PacMan"
    
    if mkdir -p "$project_dir"/{src,include}; then
        print_success "项目目录创建成功: $project_dir"
        print_info "目录结构:"
        tree "$project_dir" 2>/dev/null || ls -la "$project_dir"
    else
        print_error "项目目录创建失败"
        exit 1
    fi
}

# 创建测试Makefile
create_test_makefile() {
    print_info "创建测试Makefile..."
    local project_dir="./PacMan"
    
    cat > "$project_dir/Makefile" << 'EOF'
# 测试Makefile for PacMan项目
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LIBS = -lsx -lX11 -lXt
INCLUDE = -I/usr/local/include -I./include
LIBPATH = -L/usr/local/lib

# 测试目标
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

clean:
	rm -f *.o test test.c

.PHONY: test clean
EOF
    
    if [[ -f "$project_dir/Makefile" ]]; then
        print_success "测试Makefile创建成功"
    else
        print_error "测试Makefile创建失败"
    fi
}

# 主函数
main() {
    echo "==========================================="
    echo "    吃豆人游戏开发环境自动搭建脚本"
    echo "==========================================="
    echo
    
    check_ubuntu
    check_network
    update_packages
    install_build_tools
    install_x11_libs
    install_libsx
    create_project_dir
    create_test_makefile
    
    echo
    echo "==========================================="
    print_success "环境搭建完成！"
    echo "==========================================="
    echo
    print_info "接下来的步骤:"
    echo "1. 进入项目目录: cd ./PacMan"
    echo "2. 测试编译环境: make test"
    echo "3. 开始开发您的吃豆人游戏！"
    echo
    print_info "如果遇到问题，请检查:"
    echo "- 确保所有依赖库都已正确安装"
    echo "- 检查 /usr/local/lib 和 /usr/local/include 目录"
    echo "- 运行 'sudo ldconfig' 更新库缓存"
    echo
}

# 错误处理
trap 'print_error "脚本执行过程中发生错误，请检查上面的错误信息"' ERR

# 执行主函数
main "$@"
