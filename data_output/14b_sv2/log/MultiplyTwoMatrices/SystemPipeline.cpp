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
    tb.o_mat1(channel_mat1);
    tb.o_mat2(channel_mat2);
    tb.i_result(channel_result);

    dut.i_mat1(channel_mat1);
    dut.i_mat2(channel_mat2);
    dut.o_result(channel_result);
    /* === Variable Section End === */
}
