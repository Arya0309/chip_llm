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
    tb.o_a_real(channel_a_real);
    tb.o_a_imag(channel_a_imag);
    tb.i_result_real(channel_result_real);
    tb.i_result_imag(channel_result_imag);

    dut.i_a_real(channel_a_real);
    dut.i_a_imag(channel_a_imag);
    dut.o_result_real(channel_result_real);
    dut.o_result_imag(channel_result_imag);
    /* === Variable Section End === */
}
