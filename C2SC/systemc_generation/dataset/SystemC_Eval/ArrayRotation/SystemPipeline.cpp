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
	tb.o_d(channel_d);
	tb.o_in_array(channel_in_array);
	tb.i_out_array(channel_out_array);

	dut.i_d(channel_d);
	dut.i_in_array(channel_in_array);
	dut.o_out_array(channel_out_array);
	/* === Variable Section End === */

}
