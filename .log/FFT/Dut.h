#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct Complex {
    double real;
    double imag;
};

class Dut : public sc_module {
public:
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<double> i_a_real[8];
    sc_fifo_in<double> i_a_imag[8];
    sc_fifo_out<double> o_result_real[8];
    sc_fifo_out<double> o_result_imag[8];

    SC_HAS_PROCESS(Dut);
    Dut(sc_module_name n);
    ~Dut() = default;

private:
    void do_fft();
};

#endif // DUT_H_