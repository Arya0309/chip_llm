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
    tb.o_w1(channel_w1);
    tb.o_b1(channel_b1);
    tb.o_w2(channel_w2);
    tb.o_b2(channel_b2);
    tb.i_output(channel_output);

    dut.i_input(channel_input);
    dut.i_w1(channel_w1);
    dut.i_b1(channel_b1);
    dut.i_w2(channel_w2);
    dut.i_b2(channel_b2);
    dut.o_output(channel_output);
    /* === Variable Section End === */
}
