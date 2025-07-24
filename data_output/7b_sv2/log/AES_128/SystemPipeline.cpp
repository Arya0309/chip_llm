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
    tb.o_plaintext(channel_plaintext);
    tb.o_key(channel_key);
    tb.i_ciphertext(channel_ciphertext);

    dut.i_plaintext(channel_plaintext);
    dut.i_key(channel_key);
    dut.o_ciphertext(channel_ciphertext);
    /* === Variable Section End === */
}
