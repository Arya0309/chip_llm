#ifndef DUT_H_
#define DUT_H_

#include <systemc>
#include <vector>
using namespace sc_core;
using namespace std;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  sc_fifo_in<vector<int>> i_v;
  sc_fifo_in<int> i_target;
  sc_fifo_out<bool> o_result;

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_