
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct InputData {
    const int* data;
    int size;
};

struct OutputData {
    int* data;
    int& size;
};

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<InputData> i_input;
  sc_fifo_in<int> i_in_h;
  sc_fifo_in<int> i_in_w;
  sc_fifo_in<int> i_pool_h;
  sc_fifo_in<int> i_pool_w;
  sc_fifo_in<int> i_stride_h;
  sc_fifo_in<int> i_stride_w;
  sc_fifo_in<OutputData> i_output;
  sc_fifo_out<int> o_out_h;
  sc_fifo_out<int> o_out_w;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
