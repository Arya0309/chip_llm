#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
#ifndef NATIVE_SYSTEMC
	cynw_p2p<int>::in i_d;
  cynw_p2p<int>::in i_in_array;
	cynw_p2p<int>::out o_out_array;
#else
  sc_fifo_in<int> i_d;
  sc_fifo_in<int> i_in_array;
  sc_fifo_out<int> o_out_array;
#endif
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
#endif
