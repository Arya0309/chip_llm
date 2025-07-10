#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define Rcon 0x01
#define sbox 0x02

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<unsigned char> i_in[16];
  sc_fifo_in<unsigned char> i_key[16];
  sc_fifo_out<unsigned char> o_out[16];
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
