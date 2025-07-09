
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct InputData {
    const float* data;
    int width;
    int height;
};

struct OutputData {
    float* data;
};

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo<InputData> i_input;
  sc_fifo<OutputData> i_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
