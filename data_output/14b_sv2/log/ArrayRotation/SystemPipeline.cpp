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
    tb.o_d(channel_d);
    tb.i_result(channel_result);

    dut.i_arr(channel_arr);
    dut.i_d(channel_d);
    dut.o_result(channel_result);
    /* === Variable Section End === */
}
