#include <systemc.h>
#include <iomanip>
#include "register_file.h"
#include "ram.h"

SC_MODULE(register_ram_tb) {
    // 信号
    sc_clock clk;
    sc_signal<sc_uint<4>> reg_rd_addr;
    sc_signal<sc_uint<4>> reg_wr_addr;
    sc_signal<sc_uint<8>> reg_wr_data;
    sc_signal<bool> reg_wr_en;
    sc_signal<sc_uint<8>> reg_rd_data;
    
    sc_signal<sc_uint<4>> ram_addr;
    sc_signal<sc_uint<8>> ram_wr_data;
    sc_signal<bool> ram_wr_en;
    sc_signal<sc_uint<8>> ram_rd_data;
    
    // 波形追踪文件
    sc_trace_file *tf;
    
    // 模块实例
    register_file reg_file;
    ram memory;
    
    // 辅助方法：十六进制显示
    void print_hex(const char* name, int value) {
        std::cout << name << ": 0x" << std::hex << std::setw(2) 
                  << std::setfill('0') << value << std::dec << std::endl;
    }
    
    // 测试流程
    void test_process() {
        // 等待初始化完成
        wait(10, SC_NS);
        std::cout << "\n===== 初始状态 =====\n";
        
        // 读取寄存器初始值
        for (int i = 0; i < 16; i++) {
            reg_rd_addr.write(i);
            wait(5, SC_NS);
            std::cout << "寄存器[" << i << "]: 0x" << std::hex << std::setw(2) 
                      << std::setfill('0') << reg_rd_data.read().to_uint() << std::dec << std::endl;
        }
        
        // 读取RAM初始值
        std::cout << "\n===== RAM初始值 =====\n";
        for (int i = 0; i < 16; i++) {
            ram_addr.write(i);
            wait(5, SC_NS);
            std::cout << "RAM[" << i << "]: 0x" << std::hex << std::setw(2) 
                      << std::setfill('0') << ram_rd_data.read().to_uint() << std::dec << std::endl;
        }
        
        // 寄存器写入测试
        std::cout << "\n===== 寄存器写入测试 =====\n";
        reg_wr_en.write(true);
        for (int i = 0; i < 16; i++) {
            reg_wr_addr.write(i);
            reg_wr_data.write(0xA0 + i);  // 写入新数据
            wait(10, SC_NS);  // 等待写入完成
        }
        reg_wr_en.write(false);
        wait(10, SC_NS);
        
        // 验证寄存器写入结果
        std::cout << "寄存器写入后的值:\n";
        for (int i = 0; i < 16; i++) {
            reg_rd_addr.write(i);
            wait(5, SC_NS);
            print_hex(("寄存器[" + std::to_string(i) + "]").c_str(), reg_rd_data.read().to_uint());
        }
        
        // RAM写入测试
        std::cout << "\n===== RAM写入测试 =====\n";
        ram_wr_en.write(true);
        for (int i = 0; i < 16; i++) {
            ram_addr.write(i);
            ram_wr_data.write(0x50 + i);  // 写入新数据
            wait(10, SC_NS);  // 等待写入完成
        }
        ram_wr_en.write(false);
        wait(10, SC_NS);
        
        // 验证RAM写入结果
        std::cout << "RAM写入后的值:\n";
        for (int i = 0; i < 16; i++) {
            ram_addr.write(i);
            wait(5, SC_NS);
            print_hex(("RAM[" + std::to_string(i) + "]").c_str(), ram_rd_data.read().to_uint());
        }
        
        // 测试数据传输（寄存器 -> RAM）
        std::cout << "\n===== 数据传输测试（寄存器 -> RAM）=====\n";
        for (int i = 0; i < 8; i++) {
            // 读寄存器
            reg_rd_addr.write(i);
            wait(5, SC_NS);
            sc_uint<8> data = reg_rd_data.read();
            
            // 写RAM
            ram_wr_en.write(true);
            ram_addr.write(15 - i);
            ram_wr_data.write(data);
            wait(10, SC_NS);
            ram_wr_en.write(false);
            wait(5, SC_NS);
            
            std::cout << "从寄存器[" << i << "]传输 0x" << std::hex << std::setw(2) 
                      << std::setfill('0') << data.to_uint() << " 到 RAM[" 
                      << (15-i) << "]\n" << std::dec;
        }
        
        // 验证传输结果
        std::cout << "\n传输后RAM的值:\n";
        for (int i = 8; i < 16; i++) {
            ram_addr.write(i);
            wait(5, SC_NS);
            print_hex(("RAM[" + std::to_string(i) + "]").c_str(), ram_rd_data.read().to_uint());
        }
        
        std::cout << "\n===== 测试完成 =====\n";
        sc_stop();
    }
    
    // 构造函数
    SC_CTOR(register_ram_tb) 
    : clk("clk", 10, SC_NS),  // 10ns周期的时钟
      reg_file("register_file_inst"),
      memory("ram_inst") {
        
        // 连接寄存器堆
        reg_file.clk(clk);
        reg_file.rd_addr(reg_rd_addr);
        reg_file.wr_addr(reg_wr_addr);
        reg_file.wr_data(reg_wr_data);
        reg_file.wr_en(reg_wr_en);
        reg_file.rd_data(reg_rd_data);
        
        // 连接RAM
        memory.clk(clk);
        memory.addr(ram_addr);
        memory.wr_data(ram_wr_data);
        memory.wr_en(ram_wr_en);
        memory.rd_data(ram_rd_data);
        
        // 注册测试进程
        SC_THREAD(test_process);
        sensitive << clk.posedge_event();  
        
        // 创建波形文件
        tf = sc_create_vcd_trace_file("register_ram");
        tf->set_time_unit(1, SC_NS);
        
        // 添加信号到波形文件
        sc_trace(tf, clk, "clk");
        
        sc_trace(tf, reg_rd_addr, "reg_rd_addr");
        sc_trace(tf, reg_wr_addr, "reg_wr_addr");
        sc_trace(tf, reg_wr_data, "reg_wr_data");
        sc_trace(tf, reg_wr_en, "reg_wr_en");
        sc_trace(tf, reg_rd_data, "reg_rd_data");
        
        sc_trace(tf, ram_addr, "ram_addr");
        sc_trace(tf, ram_wr_data, "ram_wr_data");
        sc_trace(tf, ram_wr_en, "ram_wr_en");
        sc_trace(tf, ram_rd_data, "ram_rd_data");
    }
    
    ~register_ram_tb() {
        sc_close_vcd_trace_file(tf);
    }
};

int sc_main(int argc, char* argv[]) {
    register_ram_tb tb("testbench");
    
    // 初始化寄存器堆和RAM
    std::string mem_file = "./mem1.txt";
    tb.reg_file.initialize(mem_file);
    tb.memory.initialize(mem_file);
    
    // 开始仿真
    sc_start();
    
    return 0;
}
