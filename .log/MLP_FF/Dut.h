#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define INPUT_SIZE 128
#define HIDDEN_SIZE 64
#define OUTPUT_SIZE 10

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<float> i_input;
  sc_fifo_in<float> i_b1;
  sc_fifo_in<float> i_b2;
  sc_fifo_in<float> i_w1;
  sc_fifo_in<float> i_w2;
  sc_fifo_out<float> o_hidden;
  sc_fifo_out<float> o_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_