#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_in<int> i_width;
    sc_in<int> i_height;
    sc_in<sc_vector<float>> i_input;
    sc_out<sc_vector<float>> i_output;

    SC_HAS_PROCESS(Dut);
    Dut(sc_module_name n);
    ~Dut() = default;

private:
    void do_compute();
};

#endif // DUT_H_