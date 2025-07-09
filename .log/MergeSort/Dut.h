
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

/* === Variable Section === */
  sc_fifo_in<int> i_left;
  sc_fifo_in<int> i_mid;
  sc_fifo_in<int> i_right;
  sc_fifo_in<vector<int>> i_vec;
  sc_fifo_out<vector<int>> o_vec;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
