#ifndef DUT_H_
#define DUT_H_

#include <systemc>
#include <sc_vector>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_vector<sc_in<int>> i_a(6);
  sc_vector<sc_out<int>> o_result(6);
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_