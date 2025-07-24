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
    tb.o_train(channel_train);
    tb.o_labels(channel_labels);
    tb.o_query_point(channel_query_point);
    tb.i_result(channel_result);

    dut.i_train(channel_train);
    dut.i_labels(channel_labels);
    dut.i_query_point(channel_query_point);
    dut.o_result(channel_result);
    /* === Variable Section End === */
}
