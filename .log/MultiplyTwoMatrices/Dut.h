#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define R1 2
#define C1 2
#define R2 2
#define C2 3

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  sc_fifo_in<int> i_mat1[R1][C1];
  sc_fifo_in<int> i_mat2[R2][C2];
  sc_fifo_out<int> o_result[R1][C2];
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void mulMat();
};

#endif // DUT_H_