// File: fifo_tb.cpp
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

#include <systemc.h>
#include <iomanip>
#include <queue>
#include <random>
#include "fifo.h"

// 测试平台模块
SC_MODULE(fifo_tb) {
    // 信号定义
    sc_clock clk;
    sc_signal<bool> rst_n;
    sc_signal<bool> write_en;
    sc_signal<int> data_in;
    sc_signal<bool> read_en;
    sc_signal<int> data_out;
    sc_signal<bool> full;
    sc_signal<bool> empty;
    sc_signal<unsigned int> size;
    
    // 波形跟踪文件
    sc_trace_file *tf;
    
    // 参考模型
    std::queue<int> reference_fifo;
    
    // 被测FIFO实例
    fifo<int, 8> fifo_inst;
    
    // 测试参数
    const int MAX_TESTS = 1000;  // 最大测试次数
    const double WRITE_PROB = 0.6;  // 写入概率
    const double READ_PROB = 0.4;   // 读取概率
    
    // 随机数生成器
    std::mt19937 rng;
    std::uniform_real_distribution<double> dist;
    std::uniform_int_distribution<int> data_dist;
    
    // 测试进程
    void test_process() {
        // 初始化
        rst_n.write(false);  // 激活复位
        write_en.write(false);
        read_en.write(false);
        data_in.write(0);
        
        // 等待5个时钟周期后释放复位
        for (int i = 0; i < 5; i++) {
            wait(clk.posedge_event());
        }
        rst_n.write(true);   // 释放复位
        
        wait(clk.posedge_event());
        std::cout << "\n===== FIFO测试开始 =====\n\n";
        
        // 运行随机测试
        int test_count = 0;
        bool error_detected = false;
        
        while (test_count < MAX_TESTS && !error_detected) {
            // 决定本周期是否写入
            bool do_write = dist(rng) < WRITE_PROB && !full.read();
            
            // 决定本周期是否读取
            bool do_read = dist(rng) < READ_PROB && !empty.read();
            
            // 设置控制信号
            write_en.write(do_write);
            read_en.write(do_read);
            
            // 如果写入，生成随机数据
            if (do_write) {
                int data = data_dist(rng);
                data_in.write(data);
                reference_fifo.push(data);
            }
            
            // 等待下一个时钟上升沿
            wait(clk.posedge_event());
            
            // 让信号稳定
            wait(1, SC_NS);
            
            // 如果读取，验证数据
            if (do_read && !reference_fifo.empty()) {
                int expected = reference_fifo.front();
                reference_fifo.pop();
                
                int actual = data_out.read();
                
                // 验证读取的数据
                if (actual != expected) {
                    std::cout << "错误: 预期读取 " << expected 
                              << ", 实际读取 " << actual << std::endl;
                    error_detected = true;
                }
            }
            
            // 让状态信号稳定后再验证
            wait(1, SC_NS);
            
            // 验证full/empty/size信号
            bool expected_empty = reference_fifo.empty();
            bool expected_full = reference_fifo.size() >= 8;
            unsigned int expected_size = reference_fifo.size();
            
            if (empty.read() != expected_empty || 
                full.read() != expected_full ||
                size.read() != expected_size) {
                std::cout << "错误: 状态信号不匹配\n"
                          << "预期: empty=" << expected_empty 
                          << ", full=" << expected_full 
                          << ", size=" << expected_size << "\n"
                          << "实际: empty=" << empty.read() 
                          << ", full=" << full.read() 
                          << ", size=" << size.read() << std::endl;
                error_detected = true;
            }
            
            test_count++;
        }
        
        // 特殊案例测试：满和溢出
        std::cout << "\n===== 测试FIFO满条件 =====\n";
        
        // 先清空FIFO和参考模型
        rst_n.write(false);
        wait(clk.posedge_event());
        wait(1, SC_NS);
        rst_n.write(true);
        wait(clk.posedge_event());
        reference_fifo = std::queue<int>();  // 清空参考模型
        
        // 写入直到满
        write_en.write(true);
        read_en.write(false);
        
        for (int i = 0; i < 10; i++) {  // 尝试写入10个，但FIFO容量为8
            data_in.write(100 + i);
            if (i < 8) {
                reference_fifo.push(100 + i);
            }
            wait(clk.posedge_event());
            wait(1, SC_NS);  // 等待状态稳定
            
            std::cout << "写入循环 " << i << ", FIFO满状态: " 
                      << full.read() << ", FIFO大小: " << size.read() << std::endl;
            
            // 验证8个数据后FIFO应该满了
            if (i >= 7 && !full.read()) {
                std::cout << "错误: FIFO应该在写入8个元素后满" << std::endl;
                error_detected = true;
                break;
            }
        }
        
        // 特殊案例测试：空和读空
        std::cout << "\n===== 测试FIFO空条件 =====\n";
        
        // 读出所有数据
        write_en.write(false);
        read_en.write(true);
        
        for (int i = 0; i < 10; i++) {  // 尝试读取10个，但FIFO只有8个
            wait(clk.posedge_event());
            wait(1, SC_NS);  // 等待状态稳定
            
            std::cout << "读取循环 " << i << ", FIFO空状态: " 
                      << empty.read() << ", FIFO大小: " << size.read() << std::endl;
            
            // 验证8个数据后FIFO应该空了
            if (i >= 7 && !empty.read()) {
                std::cout << "错误: FIFO应该在读取8个元素后空" << std::endl;
                error_detected = true;
                break;
            }
        }
        
        // 测试结果
        if (error_detected) {
            std::cout << "\n===== FIFO测试失败 =====\n";
        } else {
            std::cout << "\n===== FIFO测试通过 (" << test_count << "个测试用例) =====\n";
        }
        
        // 结束仿真
        sc_stop();
    }

    // 构造函数
    SC_CTOR(fifo_tb)
    : clk("clk", 10, SC_NS),
      fifo_inst("fifo_instance"),
      rng(std::random_device()()),
      dist(0.0, 1.0),
      data_dist(0, 100) {
        
        // 连接FIFO端口
        fifo_inst.clk(clk);
        fifo_inst.rst_n(rst_n);
        fifo_inst.write_en(write_en);
        fifo_inst.data_in(data_in);
        fifo_inst.read_en(read_en);
        fifo_inst.data_out(data_out);
        fifo_inst.full(full);
        fifo_inst.empty(empty);
        fifo_inst.size(size);
        
        // 注册测试进程
        SC_THREAD(test_process);
        
        // 创建波形文件
        tf = sc_create_vcd_trace_file("fifo_sim");
        tf->set_time_unit(1, SC_NS);
        
        // 添加信号到波形
        sc_trace(tf, clk, "clk");
        sc_trace(tf, rst_n, "rst_n");
        sc_trace(tf, write_en, "write_en");
        sc_trace(tf, data_in, "data_in");
        sc_trace(tf, read_en, "read_en");
        sc_trace(tf, data_out, "data_out");
        sc_trace(tf, full, "full");
        sc_trace(tf, empty, "empty");
        sc_trace(tf, size, "size");
    }
    
    ~fifo_tb() {
        sc_close_vcd_trace_file(tf);
    }
};

// 主函数
int sc_main(int argc, char* argv[]) {
    fifo_tb tb("fifo_testbench");
    sc_start();
    return 0;
}
