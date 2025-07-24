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
    tb.o_binary(channel_binary);
    tb.i_decimal(channel_decimal);
    tb.o_binary(channel_binary);
    tb.i_decimal(channel_decimal);
    
    /*Missing dut binding*/
    // tb.o_binary(channel_binary);
    // tb.i_decimal(channel_decimal);

    // dut.i_binary(channel_binary);
    // dut.o_decimal(channel_decimal);
    /* === Variable Section End === */
}
