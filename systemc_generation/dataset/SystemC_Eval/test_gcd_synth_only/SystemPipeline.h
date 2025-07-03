#ifndef SYSTEM_PIPELINE_H_
#define SYSTEM_PIPELINE_H_

#include <systemc>
using namespace sc_core;

#include "Testbench.h"

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#include "Dut_wrap.h"
#else
#include "Dut.h"
#endif

/* === Fixed Format === */
class SystemPipeline: public sc_module
{
public:
	SC_HAS_PROCESS( SystemPipeline );
	SystemPipeline( sc_module_name n );
	~SystemPipeline() = default;
private:
  	Testbench tb;
#ifndef NATIVE_SYSTEMC
	Dut_wrapper dut;
#else
	Dut dut;
#endif

	sc_clock clk;
	sc_signal<bool> rst;
/* === Fixed Format End === */

/* === Variable Section === */
#ifndef NATIVE_SYSTEMC
	cynw_p2p<int> channel_a;
	cynw_p2p<int> channel_b;
	cynw_p2p<int> channel_result;
#else
	sc_fifo<int> channel_a;
    sc_fifo<int> channel_b;
    sc_fifo<int> channel_result;
#endif
/* === Variable Section End === */

};
#endif
