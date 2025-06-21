# PacMan 游戏项目

一个使用C语言和libsx图形库开发的简化版吃豆人游戏，采用模块化设计，支持多种智能算法和动态配置。

## 项目特色

- 🎮 **经典游戏体验**：完整实现吃豆人游戏机制
- 🧠 **智能算法**：集成多种AI算法（随机、之字形、DFS）
- ⚙️ **动态配置**：支持自定义网格大小和游戏参数
- 🖥️ **跨平台**：支持Windows和Linux系统
- 📦 **模块化设计**：清晰的代码架构，易于扩展

## 系统要求

### Linux (Ubuntu)
- GCC编译器
- libsx图形库
- X11开发库

### Windows
- MinGW或Visual Studio
- 相应的图形库支持

## 快速开始

### 1. 环境搭建（Ubuntu）

#### 自动安装（推荐）
```bash
# 赋予脚本执行权限
chmod +x setup_environment.sh

# 运行自动安装脚本
./setup_environment.sh
```

#### 手动安装
```bash
# 更新包管理器
sudo apt update

# 安装基础开发工具
sudo apt install build-essential gcc make wget tar

# 安装X11开发库
sudo apt install libx11-dev libxt-dev libxaw7-dev

# 安装libsx开发库
sudo apt-get install libsx-dev
```

### 2. 编译项目

```bash
# 进入项目目录
cd PacMan

# 编译项目
make

# 测试编译环境（可选）
make test
```

### 3. 运行游戏

#### 基本运行
```bash
# 使用默认设置运行
./pacman

# 或者使用make运行
make run
```

#### 自定义网格大小
```bash
# 使用-s参数指定网格大小
./pacman -s 25 20

# 直接指定宽度和高度
./pacman 30 25
```

#### 查看帮助信息
```bash
# 显示使用说明
./pacman -h

# 显示版本信息
./pacman -v
```

## 游戏控制

### 基本操作
- **方向键/按钮**：控制玩家移动
- **Rejouer**：重新开始游戏
- **Aide**：显示帮助信息
- **Quit**：退出游戏

### 智能算法控制
- **Random**：启用随机移动算法
- **Zigzag**：启用之字形移动算法
- **DFS**：启用深度优先搜索算法
- **Stop Algo**：停止当前算法

## 游戏元素

| 元素 | 描述 |
|------|------|
| 🟡 | 玩家（PacMan） |
| 🔴 | 红色幽灵 |
| 🔵 | 蓝色幽灵 |
| 🟣 | 紫色幽灵 |
| 🟠 | 橙色幽灵 |
| • | 普通豆子 |
| ● | 能量豆 |
| 🍎 | 水果奖励 |
| ▓ | 墙壁 |

## 项目结构

```
PacMan/
├── src/                    # 源代码目录
│   ├── main.c             # 程序入口和参数处理
│   ├── game.c             # 游戏逻辑核心
│   ├── gui.c              # 图形界面实现
│   └── algorithms.c       # 智能算法实现
├── include/               # 头文件目录
│   ├── types.h           # 数据类型定义
│   ├── game.h            # 游戏逻辑接口
│   ├── gui.h             # 图形界面接口
│   └── algorithms.h      # 算法接口
├── obj/                  # 编译对象文件
├── Makefile             # 构建配置
└── pacman               # 可执行文件
```

## 配置选项

### 网格大小限制
- **最小尺寸**：10 x 8
- **最大尺寸**：50 x 40
- **默认尺寸**：20 x 15

### 命令行参数
```bash
使用方法: ./pacman [选项] [宽度] [高度]

选项:
  -h, --help     显示帮助信息
  -v, --version  显示版本信息
  -s, --size     指定网格大小

示例:
  ./pacman -s 25 20    # 使用-s参数
  ./pacman 30 25       # 直接指定尺寸
```

## 开发说明

### 编译选项
- 使用`-Wall -Wextra`编译选项确保代码质量
- 支持C99标准
- 包含调试信息

### 清理和重建
```bash
# 清理编译文件
make clean

# 重新编译
make
```

### 调试模式
```bash
# 使用调试模式编译
make debug

# 使用gdb调试
gdb ./pacman
```

## 算法说明

### 1. 随机算法（Random）
- 幽灵随机选择可行的移动方向
- 适合初学者体验

### 2. 之字形算法（Zigzag）
- 幽灵按照之字形模式移动
- 提供规律性的挑战

### 3. 深度优先搜索（DFS）
- 基于图论的智能路径搜索
- 幽灵会寻找到达玩家的最优路径
- 提供最高难度的挑战

## 故障排除

### 常见问题

1. **编译错误：找不到libsx**
   ```bash
   # 确保安装了libsx开发库
   sudo apt-get install libsx-dev
   ```

2. **运行时错误：无法打开显示**
   ```bash
   # 确保X11服务正在运行
   echo $DISPLAY
   ```

3. **权限错误**
   ```bash
   # 确保有执行权限
   chmod +x pacman
   ```

### 环境测试
```bash
# 运行环境测试
make test
```

如果测试通过，会显示"libsx环境配置成功!"消息。

## 贡献指南

欢迎提交Issue和Pull Request！

1. Fork本项目
2. 创建特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 开启Pull Request

## 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 作者

- 项目开发者
- 联系方式：[您的邮箱]

## 致谢

- libsx图形库开发团队
- 所有贡献者和测试者

---

**享受游戏！** 🎮