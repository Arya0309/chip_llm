
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  /* === Variable Section === */
  sc_fifo_in<double> i_input;
  sc_fifo_in<double> i_kernel;
  sc_fifo_out<double> o_output;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();

  void conv2D(
      const double* input, int in_h, int in_w,
      const double* kernel, int ker_h, int ker_w,
      double* output);
};

#endif // DUT_H_
