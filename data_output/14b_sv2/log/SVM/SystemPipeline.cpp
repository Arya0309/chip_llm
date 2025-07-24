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
    tb.o_X(channel_X);
    tb.o_y(channel_y);
    tb.o_n(channel_n);
    tb.o_d(channel_d);
    tb.o_C(channel_C);
    tb.o_lr(channel_lr);
    tb.o_epochs(channel_epochs);
    tb.i_w(channel_w);
    tb.i_b(channel_b);

    dut.i_X(channel_X);
    dut.i_y(channel_y);
    dut.i_n(channel_n);
    dut.i_d(channel_d);
    dut.i_C(channel_C);
    dut.i_lr(channel_lr);
    dut.i_epochs(channel_epochs);
    dut.o_w(channel_w);
    dut.o_b(channel_b);
    /* === Variable Section End === */
}
