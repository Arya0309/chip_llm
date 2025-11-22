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
	tb.o_in_array(channel_in_array);
	tb.i_out(channel_out);

	dut.i_in_array(channel_in_array);
	dut.o_out(channel_out);
	/* === Variable Section End === */

}
