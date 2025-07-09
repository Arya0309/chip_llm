
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_in_h;
  sc_fifo_in<int> i_in_w;
  sc_fifo_in<int> i_ker_h;
  sc_fifo_in<int> i_ker_w;
  sc_fifo_in<sc_vector<double>> i_input;
  sc_fifo_in<sc_vector<double>> i_kernel;
  sc_fifo_out<sc_vector<double>> o_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
