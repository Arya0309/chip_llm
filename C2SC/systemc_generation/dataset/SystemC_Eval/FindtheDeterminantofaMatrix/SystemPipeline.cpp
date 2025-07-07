#include "SystemPipeline.h"
SystemPipeline::SystemPipeline( sc_module_name n ): sc_module( n ), 
	tb("tb"), dut("dut"),
	clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
	tb.i_clk(clk);
	tb.o_rst(rst);
	dut.i_clk(clk);
	dut.i_rst(rst);

	/* === Variable Section === */
	tb.o_A(channel_A);
	tb.i_C(channel_C);

	dut.i_A(channel_A);
	dut.o_C(channel_C);
	/* === Variable Section End === */

}
