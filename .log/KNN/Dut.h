#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define DIM 3
#define NUM_TRAIN 6
#define K 3

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<double> i_train[NUM_TRAIN][DIM];
  sc_fifo_in<int> i_labels[NUM_TRAIN];
  sc_fifo_in<double> i_query_point[DIM];
  sc_fifo_out<int> o_result;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_