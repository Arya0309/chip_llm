#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <systemc>
using namespace sc_core;

#define N 4

struct Testcase {
    double a[N][N];
    double expected;
};

class Testbench : public sc_module {
public:
  sc_in_clk i_clk;
  sc_out<bool> o_rst;

  /* === Variable Section === */
  sc_fifo_out<double> o_a;
  sc_fifo_in<double> i_result;
  /* === Variable Section End === */

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench() = default;

private:

  void do_feed();
  void do_fetch();

};

#endif // TESTBENCH_H_