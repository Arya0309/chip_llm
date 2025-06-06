#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

    /* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
    /* === End Fixed Format: Clock and Reset Signals === */

    // Input FIFOs for real and imaginary parts of two complex numbers
    sc_fifo_in<int> i_real1;
    sc_fifo_in<int> i_imag1;
    sc_fifo_in<int> i_real2;
    sc_fifo_in<int> i_imag2;

    // Output FIFOs for result
    sc_fifo_out<int> o_real;
    sc_fifo_out<int> o_imag;

    // Constructor
    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int r1 = i_real1.read();
            int i1 = i_imag1.read();
            int r2 = i_real2.read();
            int i2 = i_imag2.read();

            int r_sum = r1 + r2;
            int i_sum = i1 + i2;

            o_real.write(r_sum);
            o_imag.write(i_sum);

            wait();
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {

    /* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
    /* === End Fixed Format: Clock and Reset Signals === */

    sc_fifo_out<int> o_real1;
    sc_fifo_out<int> o_imag1;
    sc_fifo_out<int> o_real2;
    sc_fifo_out<int> o_imag2;

    sc_fifo_in<int> i_real;
    sc_fifo_in<int> i_imag;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        // Reset
        o_rst.write(true);
        wait();
        o_rst.write(false);

        int r1 = 3, i1 = 2;
        int r2 = 9, i2 = 5;

        o_real1.write(r1);
        o_imag1.write(i1);
        o_real2.write(r2);
        o_imag2.write(i2);
        wait(1, SC_NS);

        int r_result = i_real.read();
        int i_result = i_imag.read();

        std::cout << "Sum of complex numbers: "
                  << r_result << " + i" << i_result << std::endl;
        assert(r_result == 12);
        assert(i_result == 7);

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {

    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");

    Testbench tb("tb"); 
    Dut dut("dut");

    sc_fifo<int> fifo_real1;
    sc_fifo<int> fifo_imag1;
    sc_fifo<int> fifo_real2;
    sc_fifo<int> fifo_imag2;
    sc_fifo<int> fifo_real_result;
    sc_fifo<int> fifo_imag_result;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_real1(fifo_real1);
    tb.o_imag1(fifo_imag1);
    tb.o_real2(fifo_real2);
    tb.o_imag2(fifo_imag2);
    tb.i_real(fifo_real_result);
    tb.i_imag(fifo_imag_result);

    dut.i_real1(fifo_real1);
    dut.i_imag1(fifo_imag1);
    dut.i_real2(fifo_real2);
    dut.i_imag2(fifo_imag2);
    dut.o_real(fifo_real_result);
    dut.o_imag(fifo_imag_result);

    sc_start();
    return 0;
}
