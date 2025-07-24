#include "SystemPipeline.h"

SystemPipeline::SystemPipeline(sc_module_name n) : sc_module(n),
    tb("tb"), dut("dut"),
    clk("clk", CLOCK_PERIOD, SC_NS), rst("rst")
{
    /* === Fixed Format === */
    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);
    /* === Fixed Format End === */

    /* === Variable Section === */
    tb.o_input(channel_input);
    tb.o_window_size(channel_window_size);
    tb.i_output(channel_output);

    dut.i_input(channel_input);
    dut.i_window_size(channel_window_size);
    dut.o_output(channel_output);
    /* === Variable Section End === */
}
