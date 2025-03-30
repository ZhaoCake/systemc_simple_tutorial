#ifndef REGISTER_FILE_H
#define REGISTER_FILE_H

#include <systemc.h>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>

// 16个8位寄存器的寄存器堆
SC_MODULE(register_file) {
    // 端口声明
    sc_in<bool> clk;               // 时钟
    sc_in<sc_uint<4>> rd_addr;     // 读地址（4位，可寻址16个寄存器）
    sc_in<sc_uint<4>> wr_addr;     // 写地址（4位，可寻址16个寄存器）
    sc_in<sc_uint<8>> wr_data;     // 写数据（8位）
    sc_in<bool> wr_en;             // 写使能
    sc_out<sc_uint<8>> rd_data;    // 读数据（8位）

    // 内部存储
    sc_uint<8> registers[16];      // 16个8位寄存器

    // 读操作过程（组合逻辑，不需要时钟）
    void read_process() {
        rd_data.write(registers[rd_addr.read()]);
    }

    // 写操作过程（时序逻辑，在时钟上升沿写入）
    void write_process() {
        if (clk.posedge() && wr_en.read()) {
            registers[wr_addr.read()] = wr_data.read();
        }
    }

    // 初始化寄存器
    void initialize(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            // 忽略空行或注释行
            if (line.empty() || line[0] == '#') continue;

            // 解析行数据（格式：@地址 数据）
            if (line[0] == '@') {
                std::istringstream iss(line.substr(1));
                std::string addr_str, data_str;
                iss >> addr_str >> data_str;
                
                // 将16进制字符串转换为数字
                int addr = 0, data = 0;
                std::stringstream ss_addr, ss_data;
                ss_addr << std::hex << addr_str;
                ss_addr >> addr;
                
                ss_data << std::hex << data_str;
                ss_data >> data;
                
                if (addr >= 0 && addr < 16) {
                    registers[addr] = data;
                    std::cout << "初始化寄存器[" << addr << "] = 0x" 
                              << std::hex << std::setw(2) << std::setfill('0') 
                              << data << std::dec << std::endl;
                }
            }
        }
        
        file.close();
    }

    // 构造函数
    SC_CTOR(register_file) {
        for (int i = 0; i < 16; i++) {
            registers[i] = 0;  // 默认初始化为0
        }

        // 注册进程
        SC_METHOD(read_process);
        sensitive << rd_addr;
        
        SC_METHOD(write_process);
        sensitive << clk.pos();
        
        // 注意：初始化需要在构造后调用
    }
};

#endif // REGISTER_FILE_H
