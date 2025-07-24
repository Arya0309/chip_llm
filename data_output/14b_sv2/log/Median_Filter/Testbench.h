#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

constexpr int ROWS = 5;
constexpr int COLS = 6;

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

  /* === Variable Section === */
  sc_fifo_out<int> o_input;
  sc_fifo_out<int> o_window_size;
  sc_fifo_in<int> i_output;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif // TESTBENCH_H_
