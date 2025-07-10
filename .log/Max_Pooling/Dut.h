
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct Input {
    const int* data;
    int in_h;
    int in_w;
    int pool_h;
    int pool_w;
    int stride_h;
    int stride_w;
};

struct Output {
    int* data;
    int& out_h;
    int& out_w;
    bool done;
};

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo<Input> i_input;
  sc_fifo<Output> i_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
