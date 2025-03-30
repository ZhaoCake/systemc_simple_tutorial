// File: fifo.h
// Copyright (C) 2025  ZhaoCake

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef FIFO_H
#define FIFO_H

#include <systemc.h>
#include <deque>
#include <iostream>

// 参数化FIFO模板类，使用SC_THREAD实现
template<typename T, unsigned int DEPTH = 8>
SC_MODULE(fifo) {
    // 端口声明
    sc_in<bool>  clk;          // 时钟
    sc_in<bool>  rst_n;        // 低电平有效复位
    sc_in<bool>  write_en;     // 写使能
    sc_in<T>     data_in;      // 数据输入
    sc_in<bool>  read_en;      // 读使能
    sc_out<T>    data_out;     // 数据输出
    sc_out<bool> full;         // FIFO满信号
    sc_out<bool> empty;        // FIFO空信号
    sc_out<unsigned int> size; // 当前FIFO中元素数量

    // FIFO的内部存储
    std::deque<T> buffer;
    
    // 主进程 - 合并读写操作到一个进程，避免多驱动问题
    void fifo_process() {
        // 复位逻辑
        buffer.clear();
        full.write(false);
        empty.write(true);
        size.write(0);
        data_out.write(T());
        
        while (true) {
            // 等待时钟上升沿
            wait(clk.posedge_event());
            
            // 检查复位信号（低电平有效）
            if (!rst_n.read()) {
                buffer.clear();
                full.write(false);
                empty.write(true);
                size.write(0);
                data_out.write(T());
                continue;
            }
            
            bool did_read = false;
            bool did_write = false;
            
            // 先处理读取操作
            if (read_en.read() && !empty.read()) {
                // 获取并移除队首元素
                T value = buffer.front();
                buffer.pop_front();
                data_out.write(value);
                did_read = true;
                
                // 打印调试信息
                std::cout << sc_time_stamp() << ": 读取数据 " << value 
                          << ", FIFO大小: " << buffer.size() << std::endl;
            }
            
            // 再处理写入操作（可以在同一周期既读又写）
            if (write_en.read() && !full.read()) {
                buffer.push_back(data_in.read());
                did_write = true;
                
                // 打印调试信息
                std::cout << sc_time_stamp() << ": 写入数据 " << data_in.read() 
                          << ", FIFO大小: " << buffer.size() << std::endl;
            }
            
            // 在所有操作完成后更新状态，保证状态一致性
            if (did_read || did_write) {
                // 更新empty标志
                empty.write(buffer.empty());
                
                // 更新full标志
                full.write(buffer.size() >= DEPTH);
                
                // 更新size
                size.write(buffer.size());
            }
        }
    }

    // 构造函数
    SC_CTOR(fifo) {
        // 使用单一SC_THREAD进程处理所有逻辑，避免多驱动错误
        SC_THREAD(fifo_process);
        sensitive << clk.pos();
        
        // 初始状态
        full.initialize(false);
        empty.initialize(true);
        size.initialize(0);
    }
};

#endif // FIFO_H
