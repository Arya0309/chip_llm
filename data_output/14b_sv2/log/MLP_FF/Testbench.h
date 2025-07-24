#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

constexpr int INPUT_SIZE = 32;
constexpr int HIDDEN_SIZE = 16;
constexpr int OUTPUT_SIZE = 10;

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

  /* === Variable Section === */
  sc_fifo_out<float> o_input;
  sc_fifo_out<float> o_w1;
  sc_fifo_out<float> o_b1;
  sc_fifo_out<float> o_w2;
  sc_fifo_out<float> o_b2;
  sc_fifo_in<float> i_output;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif
