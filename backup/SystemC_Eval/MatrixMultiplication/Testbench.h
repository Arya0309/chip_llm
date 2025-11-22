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
  static const int A_ROWS = 2;
  static const int A_COLS = 2;
  static const int B_ROWS = 2;
  static const int B_COLS = 3;

/* === Variable Section === */
#ifndef NATIVE_SYSTEMC
	cynw_p2p<int>::base_out o_A;
  cynw_p2p<int>::base_out o_B;
	cynw_p2p<int>::base_in i_C;
#else
  sc_fifo_out<int> o_A;
  sc_fifo_out<int> o_B;
  sc_fifo_in<int> i_C;
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
