
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct InputPacket {
    unsigned char* data;
    int width;
    int height;
};

struct OutputPacket {
    unsigned char* data;
};

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<InputPacket> i_input;
  sc_fifo_out<OutputPacket> i_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
