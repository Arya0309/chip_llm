#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
/* === End Fixed Format: Clock and Reset Signals === */

    sc_fifo_in<float> i_fahrenheit;
    sc_fifo_out<float> o_celsius;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            float f = i_fahrenheit.read();
            float c = (f - 32.0) * 5.0 / 9.0;
            o_celsius.write(c);
            wait();
        }
    }
};

// Testbench module to validate the DUT
SC_MODULE(Testbench) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
/* === End Fixed Format: Clock and Reset Signals === */

    sc_fifo_out<float> o_fahrenheit;
    sc_fifo_in<float> i_celsius;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        float input = 40.0;
        float output;

        o_fahrenheit.write(input);
        wait(1, SC_NS);
        output = i_celsius.read();

        std::cout << "Fahrenheit: " << input << " -> Celsius: " << output << std::endl;
        assert(static_cast<int>(output * 100) == static_cast<int>(4.4444 * 100));

        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {

    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");

    Testbench tb("tb");
    Dut dut("dut");

    sc_fifo<float> fifo_fahrenheit;
    sc_fifo<float> fifo_celsius;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_fahrenheit(fifo_fahrenheit);
    tb.i_celsius(fifo_celsius);

    dut.i_fahrenheit(fifo_fahrenheit);
    dut.o_celsius(fifo_celsius);

    sc_start();
    return 0;
}
