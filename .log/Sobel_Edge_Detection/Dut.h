
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

struct InputPacket {
    const unsigned char* data;
    int size;
};

struct OutputPacket {
    unsigned char* data;
    int size;
};

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo<InputPacket> i_input;
  sc_fifo<OutputPacket> i_output;
  sc_fifo<int> i_width;
  sc_fifo<int> i_height;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
