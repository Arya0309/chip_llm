#include "SystemPipeline.h"

#define N 4

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
    for (int i = 0; i < N * N; ++i) {
        tb.o_m[i](channel_m[i]);
        tb.i_result[i](channel_result[i]);
        dut.i_m[i](channel_m[i]);
        dut.o_result[i](channel_result[i]);
    }
    /* === Variable Section End === */
}
