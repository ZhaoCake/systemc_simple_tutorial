# SystemC 语法记录

## 1. 核心概念

SystemC是一个C++库，用于系统级建模、设计和验证。它扩展了C++，增加了硬件描述语言的功能。

### 1.1 基本结构

SystemC程序的基本结构包括:
```cpp
#include <systemc>

// 使用SystemC命名空间
using namespace sc_core;

// 定义模块
SC_MODULE(ModuleName) {
    // 端口声明
    sc_in<bool> input1;
    sc_out<int> output1;
    
    // 内部信号
    sc_signal<bool> internal_signal;
    
    // 子模块
    SubModule* submodule;
    
    // 构造函数
    SC_CTOR(ModuleName) {
        // 注册进程
        SC_METHOD(process_method);
        sensitive << input1;  // 敏感列表
        
        // 实例化子模块
        submodule = new SubModule("submodule_instance");
        // 连接子模块
        submodule->port1(signal1);
    }
    
    // 进程定义
    void process_method() {
        // 进程行为
    }
};

// 主函数
int sc_main(int argc, char* argv[]) {
    // 实例化顶层模块
    ModuleName top("top_instance");
    
    // 开始仿真
    sc_start(100, SC_NS);
    
    return 0;
}
```

## 2. 模块和端口

### 2.1 模块定义

模块是SystemC的基本构建块，类似于硬件设计中的组件：

```cpp
SC_MODULE(Counter) {
    // 模块定义
    // ...
};
```

### 2.2 端口类型

SystemC提供多种端口类型：

- `sc_in<T>`: 输入端口
- `sc_out<T>`: 输出端口
- `sc_inout<T>`: 双向端口
- `sc_port<IF>`: 接口端口

```cpp
sc_in<bool> clock;
sc_out<sc_uint<8>> data_out;
```

## 3. 进程和敏感列表

SystemC支持三种进程类型：

### 3.1 SC_METHOD

不消耗仿真时间的进程，每当敏感事件发生时执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_METHOD(process);
        sensitive << clock.pos();  // 对时钟上升沿敏感
    }
    
    void process() {
        // 逻辑代码
    }
};
```

### 3.2 SC_THREAD

只启动一次，可以使用wait()语句暂停执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_THREAD(process);
        sensitive << clock.pos();
    }
    
    void process() {
        while(true) {
            // 处理逻辑
            wait();  // 等待下一个时钟周期
        }
    }
};
```

### 3.3 SC_CTHREAD

专门为时钟设计的线程，仅在指定时钟边沿执行：

```cpp
SC_MODULE(Example) {
    sc_in<bool> clock;
    
    SC_CTOR(Example) {
        SC_CTHREAD(process, clock.pos());
    }
    
    void process() {
        while(true) {
            // 处理逻辑
            wait();  // 等待下一个时钟上升沿
        }
    }
};
```

## 4. 数据类型

SystemC提供多种特殊数据类型：

- `sc_bit`: 单比特
- `sc_bv<W>`: 比特向量
- `sc_int<W>`, `sc_uint<W>`: 有符号和无符号整数
- `sc_bigint<W>`, `sc_biguint<W>`: 大整数类型
- `sc_fixed<W,I>`, `sc_ufixed<W,I>`: 定点数

```cpp
sc_uint<8> counter;  // 8位无符号整数
sc_int<16> value;    // 16位有符号整数
sc_bv<32> bus;       // 32位比特向量
```

## 5. 信号和通信

### 5.1 信号定义

信号用于模块间通信：

```cpp
sc_signal<bool> control_signal;
sc_signal<sc_uint<8>> data_bus;
```

### 5.2 读写信号

```cpp
// 读信号
bool value = signal.read();

// 写信号
signal.write(new_value);
```

## 6. 仿真控制

### 6.1 启动仿真

```cpp
// 运行特定时间
sc_start(100, SC_NS);

// 无限运行直到停止
sc_start();
```

### 6.2 停止仿真

```cpp
sc_stop();  // 停止仿真
```

### 6.3 时间管理

```cpp
sc_time current_time = sc_time_stamp();  // 获取当前仿真时间
sc_time delay(10, SC_NS);                // 10纳秒延迟
```

## 7. 调试技术

### 7.1 跟踪信号

```cpp
sc_trace_file* tf = sc_create_vcd_trace_file("wave");
sc_trace(tf, signal, "signal_name");  // 跟踪信号
```

### 7.2 打印信息

```cpp
SC_REPORT_INFO("ID", "Information message");
SC_REPORT_WARNING("ID", "Warning message");
SC_REPORT_ERROR("ID", "Error message");
SC_REPORT_FATAL("ID", "Fatal error message");
```

## 8. SystemC 与 SystemVerilog/Verilog 语法对照

对于熟悉Verilog/SystemVerilog的工程师，以下对照表可帮助理解SystemC概念。

### 8.1 基本结构对照

| 概念 | SystemC | Verilog/SystemVerilog |
|------|---------|----------------------|
| 模块定义 | `SC_MODULE(ModuleName) { ... }` | `module ModuleName(...); ... endmodule` |
| 端口声明 | `sc_in<bool> clk;` | `input logic clk;` |
| | `sc_out<sc_uint<8>> data;` | `output logic [7:0] data;` |
| | `sc_inout<bool> io_pin;` | `inout logic io_pin;` |
| 信号/线网 | `sc_signal<bool> sig;` | `logic sig;` 或 `wire sig;` |
| 参数/常量 | `static const int WIDTH = 8;` | `parameter WIDTH = 8;` |
| 敏感列表 | `sensitive << clk.pos();` | `@(posedge clk)` |
| 模块实例化 | `SubModule* sub = new SubModule("sub");` | `SubModule sub(...);` |
| 连线 | `sub->in(sig);` | `.in(sig)` |
| 时间单位 | `SC_NS`, `SC_PS` | `ns`, `ps` |
| 主函数 | `int sc_main(int argc, char* argv[]) { ... }` | (没有直接对应概念，仿真器自动执行) |

### 8.2 数据类型对照

| SystemC | SystemVerilog |
|---------|--------------|
| `bool` | `logic` |
| `sc_bit` | `bit` |
| `sc_bv<N>` | `bit [N-1:0]` |
| `sc_uint<N>` | `logic unsigned [N-1:0]` |
| `sc_int<N>` | `logic signed [N-1:0]` |
| `sc_bigint<N>`, `sc_biguint<N>` | (超过64位的大整数，没有直接对应) |
| `sc_fixed<W,I>` | (需要使用实数和转换函数模拟) |

### 8.3 行为描述对照

| 功能 | SystemC | SystemVerilog |
|------|---------|---------------|
| 组合逻辑 | `SC_METHOD(process); sensitive << in1 << in2;` | `always_comb begin ... end` |
| 时序逻辑 | `SC_METHOD(process); sensitive << clk.pos();` | `always_ff @(posedge clk) begin ... end` |
| 过程延迟 | `wait(10, SC_NS);` | `#10;` |
| 条件等待 | `wait(condition_event);` | `wait(condition);` |
| 变量赋值 | `var = value;` | `var = value;` |
| 信号赋值 | `signal.write(value);` | `signal <= value;` 或 `assign signal = value;` |
| 信号读取 | `value = signal.read();` | `value = signal;` |

### 8.4 进程/过程对照

| SystemC | SystemVerilog |
|---------|---------------|
| `SC_METHOD(process)` | `always_comb` 或 `always @(*)` |
| `SC_CTHREAD(process, clk.pos())` | `always_ff @(posedge clk)` |
| `SC_THREAD(process)` | 使用`initial begin ... end`和`fork...join`的组合 |

### 8.5 并发和同步对照

| 概念 | SystemC | SystemVerilog |
|------|---------|---------------|
| 并发执行 | 多个`SC_METHOD`进程并行执行 | 多个`always`块并行执行 |
| 同步控制 | `wait()`, `notify()`, `event` | `wait fork`, `disable fork` |
| 事件同步 | `sc_event evt; evt.notify();` | 没有直接对应，使用信号变化 |
| 延时 | `wait(10, SC_NS);` | `#10;` |

### 8.6 接口和通信

| 概念 | SystemC | SystemVerilog |
|------|---------|---------------|
| 基本接口 | 端口和信号 | 端口和线网 |
| 分层通信 | 通道、接口、端口 | 接口、通道、任务/函数 |
| FIFO | `sc_fifo<T>` | 需自行实现或使用类 |
| 回调 | 事件通知和事件查找器 | 任务调用、fork-join |

### 8.7 仿真控制

| 功能 | SystemC | SystemVerilog |
|------|---------|---------------|
| 启动仿真 | `sc_start();` | (由仿真器控制) |
| 结束仿真 | `sc_stop();` | `$finish;` |
| 打印调试 | `std::cout << msg;` | `$display("%s", msg);` |
| 转储波形 | `sc_trace(file, signal, name);` | `$dumpvars;` |
| 时间控制 | `sc_time_stamp()` | `$time` |

### 8.8 测试环境

| 概念 | SystemC | SystemVerilog |
|------|---------|---------------|
| 测试台 | 自定义C++类 | `module testbench` |
| 激励生成 | C++函数或类方法 | 任务和函数 |
| 检查结果 | C++ `if/else` 或断言 | `assert` 语句 |
| 覆盖率 | 需额外库支持 | 内置覆盖率指令 |
| 随机化 | C++标准库或自定义 | 内置约束随机化 |

### 8.9 代码风格区别

1. **抽象级别**：
   - SystemC：更高级，面向对象，适合系统级和算法级建模
   - SystemVerilog：更贴近硬件，适合RTL和门级建模

2. **语法风格**：
   - SystemC：C++语法，使用`.`和`->`访问成员
   - SystemVerilog：HDL语法，使用`.`访问成员

3. **编译与执行**：
   - SystemC：需要C++编译器，生成可执行文件
   - SystemVerilog：需要HDL仿真器直接解释执行

4. **扩展能力**：
   - SystemC：可以利用全部C++特性
   - SystemVerilog：语言扩展受限于HDL规范

### 8.10 示例对比

**例1：简单计数器**

SystemC:
```cpp
SC_MODULE(Counter) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_out<sc_uint<8>> count;
    
    sc_uint<8> counter;
    
    SC_CTOR(Counter) {
        SC_METHOD(process);
        sensitive << clk.pos() << rst;
        counter = 0;
    }
    
    void process() {
        if (rst.read()) {
            counter = 0;
        } else if (clk.pos()) {
            counter++;
        }
        count.write(counter);
    }
};
```

SystemVerilog:
```systemverilog
module Counter(
    input  logic       clk,
    input  logic       rst,
    output logic [7:0] count
);
    always_ff @(posedge clk or posedge rst) begin
        if (rst) 
            count <= 8'd0;
        else
            count <= count + 1'b1;
    end
endmodule
```

**例2：简单状态机**

SystemC:
```cpp
SC_MODULE(FSM) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<bool> input;
    sc_out<bool> output;
    
    enum State { S0, S1, S2 };
    State state;
    
    SC_CTOR(FSM) {
        SC_METHOD(state_logic);
        sensitive << clk.pos() << rst;
        state = S0;
    }
    
    void state_logic() {
        if (rst.read()) {
            state = S0;
            output.write(false);
        } else if (clk.pos()) {
            switch (state) {
                case S0:
                    state = input.read() ? S1 : S0;
                    output.write(false);
                    break;
                case S1:
                    state = input.read() ? S2 : S0;
                    output.write(false);
                    break;
                case S2:
                    state = input.read() ? S2 : S0;
                    output.write(true);
                    break;
            }
        }
    }
};
```

SystemVerilog:
```systemverilog
module FSM (
    input  logic clk,
    input  logic rst,
    input  logic input_signal,
    output logic output_signal
);
    typedef enum logic [1:0] {S0, S1, S2} state_t;
    state_t state;
    
    always_ff @(posedge clk or posedge rst) begin
        if (rst) begin
            state <= S0;
            output_signal <= 1'b0;
        end else begin
            case (state)
                S0: begin
                    state <= input_signal ? S1 : S0;
                    output_signal <= 1'b0;
                end
                S1: begin
                    state <= input_signal ? S2 : S0;
                    output_signal <= 1'b0;
                end
                S2: begin
                    state <= input_signal ? S2 : S0;
                    output_signal <= 1'b1;
                end
            endcase
        end
    end
endmodule
```

