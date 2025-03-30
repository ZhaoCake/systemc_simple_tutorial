# 实验四：FIFO设计与验证

本实验实现一个参数化的FIFO（First In, First Out）先进先出缓冲器，深入探索SystemC中的SC_THREAD进程模型和时序控制。

## 实验目标

1. 理解并实现使用SC_THREAD进程的时序设计
2. 掌握SystemC中的wait()语句用法
3. 学习如何进行对照模型验证
4. 实现并验证满/空条件的正确处理

## FIFO设计原理

FIFO是一种重要的硬件缓冲结构，用于在不同速率的模块间传输数据。其特点是：

1. **先进先出**：最先写入的数据最先被读出
2. **满/空状态**：表示缓冲区的极限状态
3. **异步接口**：支持同时读写操作

### FIFO核心操作

FIFO支持两种基本操作：
- **写入(Write)**：当FIFO不满时，数据被添加到队尾
- **读取(Read)**：当FIFO不空时，从队首取出数据

### 控制信号

- **满(Full)**：表示FIFO已达到最大深度，无法再写入
- **空(Empty)**：表示FIFO中没有数据，无法读取
- **大小(Size)**：当前存储的数据量

## SC_THREAD与SC_METHOD的对比

在本实验中，我们使用SC_THREAD而非前几个实验中的SC_METHOD，这两种SystemC进程模型有显著区别：

| 特性 | SC_METHOD | SC_THREAD |
|------|-----------|-----------|
| 执行模式 | 每次触发完整执行 | 可以暂停和恢复 |
| 时间控制 | 不能等待时间 | 可以使用wait()等待 |
| 适用场景 | 组合逻辑、简单时序 | 复杂时序、协议建模 |
| 状态保存 | 需要显式变量 | 自动保存执行状态 |
| 代码风格 | 离散事件处理 | 类似连续流程描述 |

## 实现要点

### 1. SC_THREAD与wait()

使用SC_THREAD进程模型和wait()语句实现时序控制：

```cpp
SC_THREAD(write_process);
sensitive << clk.pos();  // 初始敏感信号

void write_process() {
    // 初始化代码
    while (true) {
        wait(clk.posedge_event());  // 等待时钟上升沿
        // 本周期的逻辑...
    }
}
```

这种模式下，进程第一次执行到wait()时会暂停，直到指定事件(时钟上升沿)发生后再继续执行wait()后面的代码。

### 2. 复位逻辑

复位逻辑放置在循环开始处，确保系统可以在任何时刻复位：

```cpp
while (true) {
    wait(clk.posedge_event());
    
    if (!rst_n.read()) {  // 低电平有效复位
        buffer.clear();
        empty.write(true);
        full.write(false);
        size.write(0);
        continue;  // 跳过本周期其他操作
    }
    
    // 正常操作...
}
```

### 3. 读写操作与状态更新

FIFO实现中的一个关键点是保证状态信号(empty/full/size)与实际缓冲区状态一致：

```cpp
// 先处理读取和写入操作
bool did_read = false;
bool did_write = false;

if (read_en.read() && !empty.read()) {
    // 读取操作...
    did_read = true;
}

if (write_en.read() && !full.read()) {
    // 写入操作...
    did_write = true;
}

// 在所有操作完成后统一更新状态
if (did_read || did_write) {
    empty.write(buffer.empty());
    full.write(buffer.size() >= DEPTH);
    size.write(buffer.size());
}
```

这种模式确保了：
1. 状态更新只在有实际读/写操作时发生
2. 所有状态更新都在操作后同时进行，保持一致性
3. 在同一个周期中进行的操作会被正确处理

### 4. 使用C++标准库

本实验中使用C++标准库中的`std::deque`实现FIFO缓冲区，展示了SystemC与C++无缝集成的优势：

```cpp
std::deque<T> buffer;  // 使用模板参数T
```

## 测试平台设计

测试平台采用对照测试方法，同时使用一个参考模型(reference_fifo)执行相同操作，然后比较结果：

```cpp
// 参考模型
std::queue<int> reference_fifo;

// 如果写入，同时更新被测FIFO和参考模型
if (do_write) {
    int data = data_dist(rng);
    data_in.write(data);
    reference_fifo.push(data);
}

// 如果读取，比较被测FIFO和参考模型的输出
if (do_read) {
    int expected = reference_fifo.front();
    reference_fifo.pop();
    int actual = data_out.read();
    
    // 验证读取的数据
    if (actual != expected) {
        // 报错...
    }
}
```

### 测试过程中的时序考虑

在测试时，需要确保给FIFO足够的时间来更新其内部状态和输出：

```cpp
// 设置输入信号
write_en.write(do_write);
read_en.write(do_read);

// 等待时钟上升沿，让FIFO处理输入
wait(clk.posedge_event());

// 等待一小段时间，让输出信号稳定
wait(1, SC_NS);

// 现在可以安全地验证输出和状态信号
// ...验证代码...
```

这种时序安排模拟了实际硬件中的信号传播延迟，确保了测试的准确性。

## 验证策略

测试平台采用以下策略验证FIFO功能：

1. **随机测试**：随机生成读写操作和数据值
2. **边界测试**：专门测试FIFO满/空的极限情况
3. **状态验证**：验证full/empty/size信号的正确性
4. **数据正确性**：验证读出的数据与参考模型一致

## 常见错误和调试技巧

1. **状态不一致**：最常见的FIFO实现错误是状态信号(empty/full/size)与实际缓冲区状态不一致，应在所有操作后一次性更新所有状态。

2. **边界条件处理**：FIFO为空时读取或满时写入需要特别注意，确保这些情况被正确处理。

3. **时序相关问题**：在波形查看器中检查信号变化的时序关系，确保状态更新发生在正确的时钟沿后。

4. **参考模型同步**：确保参考模型的操作与FIFO模型严格同步，尤其是在边界条件下。

## 学习要点

通过本实验，您应该能够理解：

1. **SC_THREAD的优势**：如何使用SC_THREAD实现复杂的时序行为
2. **wait()机制**：如何使用wait()创建时序依赖
3. **进程分离**：如何将读写操作分离到不同进程
4. **状态管理**：如何管理FIFO的满/空状态和计数
5. **参考模型验证**：如何使用对照测试方法验证设计

## 进阶练习

1. 修改FIFO实现，使用数组而非std::deque
2. 添加几乎满(almost_full)和几乎空(almost_empty)阈值信号
3. 实现异步FIFO，读写端使用不同时钟域
4. 增加数据有效性验证功能，如奇偶校验

## 总结

本实验通过FIFO设计展示了SystemC中SC_THREAD进程的强大功能，特别适合于实现复杂的时序行为和状态机。与前面实验中的SC_METHOD相比，SC_THREAD提供了更自然的编程模型，特别适合于测试平台和复杂协议的建模。
