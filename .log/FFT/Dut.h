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

  sc_in<int> i_n;
  sc_in<sc_lv<32>> i_real[1024];
  sc_in<sc_lv<32>> i_imag[1024];

  sc_out<sc_lv<32>> o_real[1024];
  sc_out<sc_lv<32>> o_imag[1024];

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_fft();
};

#endif // DUT_H_