#ifndef SYSTEM_PIPELINE_H_
#define SYSTEM_PIPELINE_H_

#include <systemc>
using namespace sc_core;

#include "Testbench.h"

#include "Dut.h"

/* === Fixed Format === */
class SystemPipeline: public sc_module
{
public:
	SC_HAS_PROCESS( SystemPipeline );
	SystemPipeline( sc_module_name n );
	~SystemPipeline() = default;
private:
  	Testbench tb;
	Dut dut;
	sc_clock clk;
	sc_signal<bool> rst;
/* === Fixed Format End === */

/* === Variable Section === */
	sc_fifo<int> channel_a;
    sc_fifo<int> channel_b;
    sc_fifo<int> channel_result;
/* === Variable Section End === */

};
#endif
