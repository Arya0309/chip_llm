
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define DIM 3
#define NUM_TRAIN 10
#define K 5

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<double[DIM][NUM_TRAIN]> i_train;
  sc_fifo_in<int[NUM_TRAIN]> i_labels;
  sc_fifo_in<int> i_num_train;
  sc_fifo_in<double[DIM]> i_query_point;
  sc_fifo_out<int> o_result;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};
