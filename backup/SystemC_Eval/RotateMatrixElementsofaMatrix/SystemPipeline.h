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

/* === Variable Section === */
#ifndef NATIVE_SYSTEMC
	cynw_p2p<int> channel_A;
	cynw_p2p<int> channel_C;
#else
	sc_fifo<int> channel_A;
    sc_fifo<int> channel_C;
#endif
/* === Variable Section End === */

};
#endif
