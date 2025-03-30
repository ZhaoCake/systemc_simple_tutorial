#ifndef MUX_4TO1_H
#define MUX_4TO1_H

#include <systemc.h>

SC_MODULE(mux_4to1) {
    // 输入端口
    sc_in<sc_uint<2>> X0;    // 2位输入0
    sc_in<sc_uint<2>> X1;    // 2位输入1
    sc_in<sc_uint<2>> X2;    // 2位输入2
    sc_in<sc_uint<2>> X3;    // 2位输入3
    sc_in<sc_uint<2>> Y;     // 2位选择输入
    
    // 输出端口
    sc_out<sc_uint<2>> F;    // 2位输出
    
    void mux_process() {
        switch (Y.read()) {
            case 0: F.write(X0.read()); break;  // Y=00, 选择X0
            case 1: F.write(X1.read()); break;  // Y=01, 选择X1
            case 2: F.write(X2.read()); break;  // Y=10, 选择X2
            case 3: F.write(X3.read()); break;  // Y=11, 选择X3
            default: F.write(0); break;         // 默认情况（不应发生）
        }
    }
    
    // 构造函数
    SC_CTOR(mux_4to1) {
        // 注册进程
        SC_METHOD(mux_process);
        sensitive << X0 << X1 << X2 << X3 << Y;  // 对所有输入敏感
    }
};

#endif // MUX_4TO1_H
