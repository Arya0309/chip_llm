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
    tb.o_arr(channel_arr);
    tb.i_sorted_arr(channel_sorted_arr);

    dut.i_arr(channel_arr);
    dut.o_sorted_arr(channel_sorted_arr);
    /* === Variable Section End === */
}
