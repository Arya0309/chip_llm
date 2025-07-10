#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_arr0;
  sc_fifo_in<int> i_arr1;
  sc_fifo_in<int> i_arr2;
  sc_fifo_in<int> i_arr3;
  sc_fifo_in<int> i_arr4;
  sc_fifo_in<int> i_arr5;
  sc_fifo_in<int> i_d;
  sc_fifo_out<int> o_result0;
  sc_fifo_out<int> o_result1;
  sc_fifo_out<int> o_result2;
  sc_fifo_out<int> o_result3;
  sc_fifo_out<int> o_result4;
  sc_fifo_out<int> o_result5;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_