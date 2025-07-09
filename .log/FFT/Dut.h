
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

/* === Variable Section === */
  sc_fifo_in<int> i_n;
  sc_fifo_in<double> i_real;
  sc_fifo_in<double> i_imag;
  sc_fifo_out<double> o_real;
  sc_fifo_out<double> o_imag;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
