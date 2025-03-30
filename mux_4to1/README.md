# 实验一：2位4选1选择器

本实验实现一个2位4选1选择器，理解SystemC的基本模块设计和测试方法。

## 实验要求

实现一个2位4选1的选择器，选择器有5个2位输入端，分别为X0, X1, X2, X3和Y，输出端为F；
X0, X1, X2, X3是四个2位的输入变量。输出F端受控制端Y的控制，选择其中的一个X输出：

- 当Y = 00时，输出端输出X0，即F = X0
- 当Y = 01时，输出端输出X1，即F = X1
- 当Y = 10时，输出端输出X2，即F = X2
- 当Y = 11时，输出端输出X3，即F = X3

## 实验原理

2位4选1选择器本质上是一个多路复用器，通过选择信号Y来控制将哪一组输入信号传递到输出端。其真值表如下：

| Y[1:0] | F[1:0] |
|--------|--------|
| 00     | X0     |
| 01     | X1     |
| 10     | X2     |
| 11     | X3     |

## SystemC实现思路

在SystemC中，我们可以通过以下步骤实现这个选择器：

1. 定义模块结构和端口
2. 实现选择逻辑
3. 创建测试平台来验证功能

### 模块定义

首先，我们需要创建一个继承自`sc_module`的类来表示我们的选择器模块：

```cpp
SC_MODULE(mux_4to1) {
    // 输入端口声明
    sc_in<sc_uint<2>> X0, X1, X2, X3, Y;
    
    // 输出端口声明
    sc_out<sc_uint<2>> F;
    
    // 处理方法声明
    void mux_process();
    
    // 构造函数
    SC_CTOR(mux_4to1) {
        // 注册处理方法
        SC_METHOD(mux_process);
        // 设置敏感信号
        sensitive << X0 << X1 << X2 << X3 << Y;
        // 敏感信号的顺序在SystemC中不会影响功能
        // 只要包含了所有需要监听的信号即可
    }
};
```

### 选择逻辑实现

选择逻辑可以通过switch-case语句或if-else语句实现。我们选择switch-case来增加代码清晰度：

```cpp
void mux_process() {
    switch (Y.read()) {
        case 0: F.write(X0.read()); break;  // Y=00, 选择X0
        case 1: F.write(X1.read()); break;  // Y=01, 选择X1
        case 2: F.write(X2.read()); break;  // Y=10, 选择X2
        case 3: F.write(X3.read()); break;  // Y=11, 选择X3
        default: F.write(0); break;         // 默认情况（不应发生）
    }
}
```

### 数据类型选择

我们使用`sc_uint<2>`类型，这是SystemC提供的2位无符号整数类型。这个类型正好适合表示我们需要的2位输入和输出信号。

### 进程类型选择

在SystemC中，有几种不同的进程类型：
- `SC_METHOD`：无状态的组合逻辑处理，不会自行暂停
- `SC_THREAD`：可以包含等待语句的顺序处理
- `SC_CTHREAD`：时钟驱动的线程处理

由于选择器是一个简单的组合逻辑电路，我们选择使用`SC_METHOD`，它最适合实现无状态的组合逻辑功能。

## 测试平台设计

测试平台需要完成以下任务：
1. 实例化被测模块
2. 生成测试输入
3. 观察和验证输出

### 测试信号和模块实例

```cpp
SC_MODULE(mux_4to1_tb) {
    // 测试用信号
    sc_signal<sc_uint<2>> X0_sig, X1_sig, X2_sig, X3_sig, Y_sig, F_sig;
    
    // 被测模块实例
    mux_4to1 mux_inst;
    
    // 测试进程
    void test_process();
    
    // 构造函数
    SC_CTOR(mux_4to1_tb) : mux_inst("mux_instance") {
        // 连接信号到被测模块
        mux_inst.X0(X0_sig);
        mux_inst.X1(X1_sig);
        mux_inst.X2(X2_sig);
        mux_inst.X3(X3_sig);
        mux_inst.Y(Y_sig);
        mux_inst.F(F_sig);
        
        // 注册测试进程
        SC_THREAD(test_process);
    }
};
```

### 测试逻辑

对于测试逻辑，我们使用`SC_THREAD`而不是`SC_METHOD`，因为测试需要等待时间推进：

```cpp
void test_process() {
    // 初始化输入
    X0_sig.write(0);  // 00
    X1_sig.write(1);  // 01
    X2_sig.write(2);  // 10
    X3_sig.write(3);  // 11
    
    // 逐个测试所有选择可能
    for (int i = 0; i < 4; i++) {
        Y_sig.write(i);
        wait(10, SC_NS);  // 等待信号传播
        
        // 输出结果并验证
        cout << sc_time_stamp() << "\tY = " << Y_sig.read() 
             << "\tF = " << F_sig.read() << endl;
             
        // 验证输出是否与期望相符
        sc_assert(F_sig.read() == i);
    }
    
    // 测试完成，停止仿真
    sc_stop();
}
```

### sc_main函数

最后，我们需要提供`sc_main`函数作为SystemC仿真的入口点：

```cpp
int sc_main(int argc, char* argv[]) {
    // 创建测试平台实例
    mux_4to1_tb tb("testbench");
    
    // 启动仿真
    sc_start();
    
    return 0;
}
```

## 波形输出与查看

本实验使用SystemC的波形追踪功能，将仿真结果保存为VCD格式的波形文件。VCD (Value Change Dump) 是一种标准的波形记录格式，可以被多种波形查看器读取。

### 波形输出实现

在测试平台中，我们添加了波形记录相关代码：

```cpp
// 波形追踪文件指针
sc_trace_file *tf;

// 构造函数中创建波形文件
tf = sc_create_vcd_trace_file("mux_4to1");
tf->set_time_unit(1, SC_NS);

// 添加信号到波形文件
sc_trace(tf, X0_sig, "X0");
sc_trace(tf, X1_sig, "X1");
sc_trace(tf, X2_sig, "X2");
sc_trace(tf, X3_sig, "X3");
sc_trace(tf, Y_sig, "Y");
sc_trace(tf, F_sig, "F");

// 析构函数中关闭波形文件
~mux_4to1_tb() {
    sc_close_vcd_trace_file(tf);
}
```

### 查看波形文件

运行测试后，会在当前目录生成`mux_4to1.vcd`文件。可以使用以下工具查看该波形文件：

1. **GTKWave**: 一个开源的波形查看器
   ```bash
   gtkwave mux_4to1.vcd
   ```

2. **Modelsim/QuestaSim**: 专业FPGA/ASIC仿真工具
   ```bash
   vsim -view mux_4to1.vcd
   ```

3. **Verdi**: Synopsys的调试和可视化工具
   ```bash
   verdi -ssf mux_4to1.vcd
   ```

波形查看可以帮助直观理解电路行为，尤其在复杂的时序电路设计中更为重要。

### 波形分析

观察波形，您应该能看到如下变化：

1. Y信号从0逐渐变化到3
2. F信号在每次Y变化后跟随变为对应的输入值
3. 信号变化之间有10ns的时间间隔，与`wait(10, SC_NS)`语句对应

这些观察验证了我们的选择器工作正常，根据选择信号Y的值输出正确的输入信号。

## 验证和分析

运行测试后，我们应该看到如下输出：

```
==== 2位4选1选择器测试 ====
时间    Y    F
------------------------
10 ns    0    0
20 ns    1    1
30 ns    2    2
40 ns    3    3
测试成功完成!
```

这表明我们的选择器根据Y的值正确地选择了对应的输入信号。

## SystemC学习要点

通过本实验，您应该理解了以下SystemC的核心概念：

1. **模块定义**：使用`SC_MODULE`宏定义模块
2. **端口声明**：使用`sc_in`和`sc_out`模板类声明输入输出端口
3. **数据类型**：使用`sc_uint<N>`定义N位无符号整数信号
4. **进程注册**：使用`SC_METHOD`和`SC_THREAD`注册进程
5. **敏感列表**：使用`sensitive`指定触发进程的信号
6. **信号读写**：使用`read()`和`write()`方法读写信号
7. **仿真控制**：使用`sc_start()`启动仿真和`sc_stop()`停止仿真
8. **波形记录**：使用`sc_trace_file`和`sc_trace`记录信号波形

## 进阶练习

1. 修改实现，使用if-else结构代替switch-case
2. 拓展为4位8选1选择器
3. 添加使能信号，当使能为低时输出恒为0
4. 使用波形查看器（如GTKWave）可视化仿真波形

## 参考资料

- SystemC语言参考手册：https://www.accellera.org/downloads/standards/systemc
- SystemC用户指南：https://www.accellera.org/images/downloads/standards/systemc/IEEE1666_2011_SystemC_Standard.pdf
- GTKWave用户手册：http://gtkwave.sourceforge.net/gtkwave.pdf
