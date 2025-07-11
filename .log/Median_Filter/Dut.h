#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define ROWS 5
#define COLS 6

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_input;
  sc_fifo_in<int> i_rows;
  sc_fifo_in<int> i_cols;
  sc_fifo_in<int> i_window_size;
  sc_fifo_out<int> o_output;
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_median_filter();
};

#endif // DUT_H_