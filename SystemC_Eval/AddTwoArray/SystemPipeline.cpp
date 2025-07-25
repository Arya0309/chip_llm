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
	tb.o_a(channel_a);
	tb.o_b(channel_b);
	tb.i_c(channel_c);

	dut.i_a(channel_a);
	dut.i_b(channel_b);
	dut.o_c(channel_c);
	/* === Variable Section End === */

}
