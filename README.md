# SystemC 实践教程

> 本人出于需要初次接触SystemC，深感SystemC的中文资料十分缺乏。斗胆作此教程，如有谬误，敬请指正。

> 本教程主要作用范围是SystemC的使用，包括进行模块设计和testbench编写，关于方法学上的一切，笔者完全不懂。采用的设计实验主要是[南京大学数字逻辑与计算机组成设计实验](https://nju-projectn.github.io/dlco-lecture-note)中的选作几例。

## 环境配置

本项目使用标准的Makefile构建系统，适用于Linux/Unix环境。所有编译结果统一输出到build目录。

### 前置条件
- C++编译器(gcc/g++)
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

# 运行ALU测试
make run-alu_4bit

# 运行寄存器堆和RAM测试
make run-register_ram

# 或者直接运行编译好的可执行文件
./build/mux_4to1/mux_4to1_tb
./build/alu_4bit/alu_4bit_tb
./build/register_ram/register_ram_tb

# 清理生成的文件
make clean
```

### 项目结构

```
systemC_example/
├── build/                  # 构建输出目录（自动创建）
│   ├── mux_4to1/           # 选择器实验的构建结果
│   ├── alu_4bit/           # ALU实验的构建结果
│   ├── register_ram/       # 寄存器堆和RAM实验的构建结果
│   └── fifo_design/        # FIFO实验的构建结果
├── mux_4to1/               # 2位4选1选择器
│   ├── mux_4to1.h
│   ├── mux_4to1_tb.cpp
│   ├── Makefile
│   └── README.md
├── alu_4bit/               # 4位带符号补码ALU
│   ├── alu_4bit.h
│   ├── alu_4bit_tb.cpp
│   ├── Makefile
│   └── README.md
├── register_ram/           # 寄存器堆和RAM
│   ├── register_file.h
│   ├── ram.h
│   ├── register_ram_tb.cpp
│   ├── mem1.txt
│   ├── Makefile
│   └── README.md
├── fifo_design/            # FIFO设计与验证
│   ├── fifo.h
│   ├── fifo_tb.cpp
│   ├── Makefile
│   └── README.md
├── Makefile                # 主Makefile
└── README.md               # 项目文档
```

## 实验列表

### 实验一：2位4选1选择器
实现一个2位4选1的选择器，根据选择信号Y的值选择对应的输入信号X输出。
详情见[mux_4to1/README.md](mux_4to1/README.md)

### 实验二：4位带符号补码ALU
实现一个4位带符号位的补码算术逻辑单元，支持加减法、逻辑运算和比较操作。
详情见[alu_4bit/README.md](alu_4bit/README.md)

### 实验三：寄存器堆与RAM
实现16×8位的寄存器堆与RAM，支持读写操作和从文件初始化。
详情见[register_ram/README.md](register_ram/README.md)

### 实验四：FIFO设计与验证
实现一个参数化的FIFO缓冲器，使用SC_THREAD进程展示复杂时序行为控制。
详情见[fifo_design/README.md](fifo_design/README.md)



