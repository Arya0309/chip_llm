#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

#define R 4
#define C 4

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void rotatematrix(int m, int n, int mat[R][C]);
  void main_function();
};

#endif // DUT_H_