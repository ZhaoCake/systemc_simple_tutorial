// alu_4bit.h
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


#ifndef ALU_4BIT_H
#define ALU_4BIT_H

#include <systemc.h>

// 4位带符号补码ALU模块
SC_MODULE(alu_4bit) {
    // 输入端口
    sc_in<sc_int<4>> A;        // 4位带符号数输入A（补码表示）
    sc_in<sc_int<4>> B;        // 4位带符号数输入B（补码表示）
    sc_in<sc_uint<3>> op;      // 3位操作选择信号
    
    // 输出端口
    sc_out<sc_int<4>> result;  // 4位带符号数结果
    sc_out<bool> zero;         // 零标志
    sc_out<bool> overflow;     // 溢出标志
    sc_out<bool> carry;        // 进位标志
    
    // ALU运算处理方法
    void alu_process() {
        // 临时变量用于计算
        sc_int<4> a_val = A.read();
        sc_int<4> b_val = B.read();
        sc_int<4> res = 0;
        bool carry_flag = false;
        bool overflow_flag = false;
        
        // 根据操作码执行相应的运算
        switch(op.read()) {
            case 0: // 加法 A+B
                {
                    // 扩展位宽进行加法，以检测溢出和进位
                    sc_int<5> a_ext = a_val;
                    sc_int<5> b_ext = b_val;
                    sc_int<5> res_ext = a_ext + b_ext;
                    
                    // 检测进位（对带符号数，检查最高位的进位）
                    carry_flag = ((a_val >= 0 && b_val >= 0 && res_ext >= 8) || 
                                 (a_val < 0 && b_val < 0 && res_ext < -8));
                    
                    // 检测溢出（符号位变化错误）
                    overflow_flag = ((a_val > 0 && b_val > 0 && res_ext < 0) || 
                                    (a_val < 0 && b_val < 0 && res_ext >= 0));
                    
                    // 取结果的低4位作为最终结果
                    res = sc_int<4>(res_ext);
                }
                break;
                
            case 1: // 减法 A-B
                {
                    // 扩展位宽进行减法，以检测溢出和进位
                    sc_int<5> a_ext = a_val;
                    sc_int<5> b_ext = b_val;
                    sc_int<5> res_ext = a_ext - b_ext;
                    
                    // 检测借位（对于减法，借位相当于有一个负的进位）
                    carry_flag = ((a_val >= 0 && b_val < 0 && res_ext >= 8) || 
                                 (a_val < 0 && b_val >= 0 && res_ext < -8));
                    
                    // 检测溢出（符号位变化错误）
                    overflow_flag = ((a_val >= 0 && b_val < 0 && res_ext < 0) || 
                                    (a_val < 0 && b_val >= 0 && res_ext >= 0));
                    
                    // 取结果的低4位作为最终结果
                    res = sc_int<4>(res_ext);
                }
                break;
                
            case 2: // 取反 Not A
                res = ~a_val;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            case 3: // 与 A and B
                res = a_val & b_val;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            case 4: // 或 A or B
                res = a_val | b_val;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            case 5: // 异或 A xor B
                res = a_val ^ b_val;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            case 6: // 比较大小 If A<B then out=1; else out=0;
                res = (a_val < b_val) ? 1 : 0;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            case 7: // 判断相等 If A==B then out=1; else out=0;
                res = (a_val == b_val) ? 1 : 0;
                carry_flag = false;
                overflow_flag = false;
                break;
                
            default:
                res = 0;
                carry_flag = false;
                overflow_flag = false;
                break;
        }
        
        // 设置输出
        result.write(res);
        zero.write(res == 0);
        overflow.write(overflow_flag);
        carry.write(carry_flag);
    }
    
    // 构造函数
    SC_CTOR(alu_4bit) {
        SC_METHOD(alu_process);
        sensitive << A << B << op;
    }
};

#endif // ALU_4BIT_H
