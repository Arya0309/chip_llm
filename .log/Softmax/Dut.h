
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#include <vector>

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_length;
  sc_fifo_in<std::vector<double>> i_input;
  sc_fifo_out<std::vector<double>> o_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
