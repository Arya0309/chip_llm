
#ifndef DUT_H_
#define DUT_H_

#include <systemc>
#include <string>
using namespace sc_core;

class Dut : public sc_module {
public:
  sc_in_clk i_clk;
  sc_in<bool> i_rst;

  sc_fifo_in<char> i_a;
  sc_fifo_in<char> i_b;
  sc_fifo_out<char> o_result;

  SC_HAS_PROCESS(Dut);
  Dut(sc_module_name n);
  ~Dut() = default;

private:
  void do_compute();
  std::string addBinary(std::string A, std::string B);
};

#endif // DUT_H_
