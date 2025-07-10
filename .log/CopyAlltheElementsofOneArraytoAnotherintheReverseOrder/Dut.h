#ifndef DUT_H_
#define DUT_H_

#include <systemc>
using namespace sc_core;

SC_MODULE(Dut) {
	sc_in_clk i_clk;
	sc_in<bool> i_rst;

	sc_fifo_in<int> i_arr;
	sc_fifo_out<int> o_copied_arr;

	SC_HAS_PROCESS(Dut);
	Dut(sc_module_name n);
	~Dut() = default;

private:
	void do_compute();
};

#endif // DUT_H_
