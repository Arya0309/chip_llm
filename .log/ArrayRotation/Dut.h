#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

void Rotate(int arr[], int d, int n);
void PrintTheArray(int arr[], int n);

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

/* === Variable Section === */
  // No variables needed for this example
/* === Variable Section End === */

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
};

#endif // DUT_H_