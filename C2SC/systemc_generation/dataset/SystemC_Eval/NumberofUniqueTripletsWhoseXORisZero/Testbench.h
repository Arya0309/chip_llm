#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

/* === Variable Section === */
#ifndef NATIVE_SYSTEMC
  cynw_p2p<int>::base_out o_in_array;
	cynw_p2p<int>::base_in i_out;
#else
  sc_fifo_out<int> o_in_array;
  sc_fifo_in<int> i_out;
#endif
/* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif
