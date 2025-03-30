#include <systemc.h>
#include "mux_4to1.h"

SC_MODULE(mux_4to1_tb) {
    // 信号
    sc_signal<sc_uint<2>> X0_sig;
    sc_signal<sc_uint<2>> X1_sig;
    sc_signal<sc_uint<2>> X2_sig;
    sc_signal<sc_uint<2>> X3_sig;
    sc_signal<sc_uint<2>> Y_sig;
    sc_signal<sc_uint<2>> F_sig;
    
    // 波形追踪文件指针
    sc_trace_file *tf;

    // 组件实例
    mux_4to1 mux_inst;
    
    // 测试进程
    void test_process() {
        // 初始化输入，每个输入赋予不同值便于观察
        X0_sig.write(0);  // 00
        X1_sig.write(1);  // 01
        X2_sig.write(2);  // 10
        X3_sig.write(3);  // 11
        
        cout << "==== 2位4选1选择器测试 ====" << endl;
        cout << "时间\tY\tF" << endl;
        cout << "------------------------" << endl;
        
        // 测试所有选择组合
        for (int i = 0; i < 4; i++) {
            Y_sig.write(i);
            wait(10, SC_NS);  // 等待信号传播
            
            cout << sc_time_stamp() << "\t" 
                 << Y_sig.read() << "\t" 
                 << F_sig.read() << endl;
            
            // 验证输出是否正确
            sc_assert(F_sig.read() == i);
        }
        
        cout << "测试成功完成!" << endl;
        cout << "波形已保存到 mux_4to1.vcd 文件" << endl;
        sc_stop();
    }
    
    // 构造函数
    SC_CTOR(mux_4to1_tb)
    : mux_inst("mux_instance") {
        // 连接信号到被测设备
        mux_inst.X0(X0_sig);
        mux_inst.X1(X1_sig);
        mux_inst.X2(X2_sig);
        mux_inst.X3(X3_sig);
        mux_inst.Y(Y_sig);
        mux_inst.F(F_sig);
        
        // 注册进程
        SC_THREAD(test_process);
        
        // 创建波形追踪文件
        tf = sc_create_vcd_trace_file("mux_4to1");
        
        // 设置波形时间单位
        tf->set_time_unit(1, SC_NS);
        
        // 添加信号到波形文件
        sc_trace(tf, X0_sig, "X0");
        sc_trace(tf, X1_sig, "X1");
        sc_trace(tf, X2_sig, "X2");
        sc_trace(tf, X3_sig, "X3");
        sc_trace(tf, Y_sig, "Y");
        sc_trace(tf, F_sig, "F");
    }
    
    // 析构函数
    ~mux_4to1_tb() {
        // 关闭波形文件
        sc_close_vcd_trace_file(tf);
    }
};

int sc_main(int argc, char* argv[]) {
    mux_4to1_tb tb("testbench");
    sc_start();
    return 0;
}
