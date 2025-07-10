#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<sc_dt::sc_vector<double>> i_X;
  sc_fifo_in<sc_dt::sc_vector<int>> i_y;
  sc_fifo_in<int> i_n;
  sc_fifo_in<int> i_d;
  sc_fifo_in<double> i_C;
  sc_fifo_in<double> i_lr;
  sc_fifo_in<int> i_epochs;
  sc_fifo_in<sc_dt::sc_vector<double>> i_w;
  sc_fifo_in<double> i_b;
  sc_fifo_out<int> o_result;
  sc_fifo_in<sc_dt::sc_vector<double>> i_x;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_