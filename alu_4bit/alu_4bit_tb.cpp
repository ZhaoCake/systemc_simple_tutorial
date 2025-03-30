// alu_4bit.cpp
// ALU 4-bit signed two's complement arithmetic logic unit
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
#include <bitset>
#include <string>
#include <iostream>
#include "alu_4bit.h"

SC_MODULE(alu_4bit_tb) {
    // 信号
    sc_signal<sc_int<4>> A_sig;
    sc_signal<sc_int<4>> B_sig;
    sc_signal<sc_uint<3>> op_sig;
    sc_signal<sc_int<4>> result_sig;
    sc_signal<bool> zero_sig;
    sc_signal<bool> overflow_sig;
    sc_signal<bool> carry_sig;
    
    // 波形追踪文件指针
    sc_trace_file *tf;
    
    // 被测试的ALU实例
    alu_4bit alu_inst;
    
    // 显示结果的辅助函数
    void display_result(sc_int<4> a, sc_int<4> b, sc_uint<3> op, 
                       sc_int<4> result, bool zero, bool overflow, bool carry) {
        const char* op_name[] = {
            "加法 (A+B)", 
            "减法 (A-B)", 
            "取反 (~A)", 
            "与 (A&B)", 
            "或 (A|B)", 
            "异或 (A^B)", 
            "比较大小 (A<B?1:0)", 
            "判断相等 (A==B?1:0)"
        };
        
        cout << "A = " << std::setw(3) << a.to_int() 
             << " (" << std::bitset<4>(a.to_uint()) << "), "
             << "B = " << std::setw(3) << b.to_int()
             << " (" << std::bitset<4>(b.to_uint()) << "), "
             << "Op = " << op.to_string()
             << " [" << op_name[op.to_uint()] << "]\n"
             << "结果: " << std::setw(3) << result.to_int()
             << " (" << std::bitset<4>(result.to_uint()) << "), "
             << "零标志: " << (zero ? "是" : "否")
             << ", 溢出标志: " << (overflow ? "是" : "否")
             << ", 进位标志: " << (carry ? "是" : "否")
             << "\n-----------------------------------------\n";
    }
    
    // 测试进程
    void test_process() {
        cout << "\n====== 4位带符号补码ALU测试 ======\n\n";
        
        // 测试用例集合
        // 格式: {操作数A, 操作数B, 操作码}
        struct {
            int a, b;
            int op;
        } test_cases[] = {
            // 加法测试用例
            {3, 4, 0},     // 正数+正数
            {-3, -4, 0},   // 负数+负数
            {7, 1, 0},     // 正溢出
            {-8, -1, 0},   // 负溢出
            
            // 减法测试用例
            {5, 3, 1},     // 正数-正数(正结果)
            {3, 5, 1},     // 正数-正数(负结果)
            {-3, -5, 1},   // 负数-负数(正结果)
            {-5, -3, 1},   // 负数-负数(负结果)
            {7, -2, 1},    // 正溢出
            {-8, 2, 1},    // 负溢出
            
            // 取反测试用例
            {5, 0, 2},     // 正数取反
            {-5, 0, 2},    // 负数取反
            
            // 逻辑运算测试用例
            {5, 3, 3},     // 与
            {5, 3, 4},     // 或
            {5, 3, 5},     // 异或
            
            // 比较和判等测试用例
            {3, 5, 6},     // 比较大小(A<B)
            {5, 3, 6},     // 比较大小(A>B)
            {-3, 5, 6},    // 比较大小(A<B，A为负)
            {3, -5, 6},    // 比较大小(A>B，B为负)
            {3, 3, 7},     // 判断相等(相等)
            {3, 4, 7},     // 判断相等(不等)
        };
        
        // 执行测试用例
        for (auto& tc : test_cases) {
            A_sig.write(tc.a);
            B_sig.write(tc.b);
            op_sig.write(tc.op);
            
            wait(10, SC_NS);
            
            display_result(
                A_sig.read(), B_sig.read(), op_sig.read(),
                result_sig.read(), zero_sig.read(), overflow_sig.read(), carry_sig.read()
            );
        }
        
        cout << "ALU测试完成！波形已保存到 alu_4bit.vcd 文件\n";
        sc_stop();
    }
    
    // 构造函数
    SC_CTOR(alu_4bit_tb)
    : alu_inst("alu_instance") {
        // 连接信号到ALU实例
        alu_inst.A(A_sig);
        alu_inst.B(B_sig);
        alu_inst.op(op_sig);
        alu_inst.result(result_sig);
        alu_inst.zero(zero_sig);
        alu_inst.overflow(overflow_sig);
        alu_inst.carry(carry_sig);
        
        // 注册测试进程
        SC_THREAD(test_process);
        
        // 创建波形追踪文件
        tf = sc_create_vcd_trace_file("alu_4bit");
        tf->set_time_unit(1, SC_NS);
        
        // 添加信号到波形文件
        sc_trace(tf, A_sig, "A");
        sc_trace(tf, B_sig, "B");
        sc_trace(tf, op_sig, "Op");
        sc_trace(tf, result_sig, "Result");
        sc_trace(tf, zero_sig, "Zero");
        sc_trace(tf, overflow_sig, "Overflow");
        sc_trace(tf, carry_sig, "Carry");
    }
    
    // 析构函数
    ~alu_4bit_tb() {
        sc_close_vcd_trace_file(tf);
    }
};

int sc_main(int argc, char* argv[]) {
    alu_4bit_tb tb("alu_testbench");
    sc_start();
    return 0;
}
