#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define n 4

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_matrix[n][n];
  sc_fifo_out<int> o_matrix[n][n];
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void interchangeFirstLast();
};

#endif // DUT_H_