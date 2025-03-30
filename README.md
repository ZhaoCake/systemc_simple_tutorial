# SystemC 实践教程

> 本人出于需要初次接触SystemC，深感SystemC的中文资料十分缺乏。斗胆作此教程，如有谬误，敬请指正。

> 本教程主要作用范围是SystemC的使用，包括进行模块设计和testbench编写，关于方法学上的一切，笔者完全不懂。采用的设计实验是南京大学数电设计实验中的选作几例。

## 环境配置

本项目使用标准的Makefile构建系统，适用于Linux/Unix环境。所有编译结果统一输出到build目录。

### 前置条件
- C++编译器(gcc/g++)，支持C++17标准
- 系统已安装的SystemC库 (头文件位于/usr/include，库文件位于/usr/lib)
- GNU Make

### 快速开始

使用Makefile编译和运行项目：

```bash
# 编译所有实验
make

# 运行所有测试
make run

# 运行特定模块的测试（如2位4选1选择器）
make run-mux_4to1

# 或者直接运行编译好的可执行文件
./build/mux_4to1/mux_4to1_tb

# 清理生成的文件
make clean
```

### 项目结构

```
systemC_example/
├── build/                  # 构建输出目录（自动创建）
│   └── mux_4to1/           # 各子项目的构建结果
├── mux_4to1/               # 2位4选1选择器
│   ├── mux_4to1.h          # 模块定义
│   ├── mux_4to1_tb.cpp     # 测试平台
│   └── Makefile            # 子项目Makefile
├── Makefile                # 主Makefile
└── README.md               # 项目文档
```

## 实验一：2位4选1选择器

### 实验要求
实现一个2位4选1的选择器，选择器有5个2位输入端，分别为X0, X1, X2, X3和Y，输出端为F；
X0, X1, X2, X3是四个2位的输入变量。输出F端受控制端Y的控制，选择其中的一个X输出：
- 当Y = 00时，输出端输出X0，即F = X0
- 当Y = 01时，输出端输出X1，即F = X1
- 当Y = 10时，输出端输出X2，即F = X2
- 当Y = 11时，输出端输出X3，即F = X3



