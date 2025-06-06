#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
/* === End Fixed Format: Clock and Reset Signals === */

    /* Dut Input ports (FIFO) */
    sc_fifo_in<int> i_a;
    sc_fifo_in<int> i_b;
    /* Dut Output ports (FIFO) */
    sc_fifo_out<int> o_result;

/* === Fixed Format: Process Registration === */
    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }
/* === End Fixed Format: Process Registration === */

/* === Main Compute Thread === */
    void do_compute() {
        while (true) {
            int a = i_a.read();
            int b = i_b.read();
            int sum = a + b;
            o_result.write(sum);
            wait(); // Wait for next clock cycle
        }
    }
/* === End Main Compute Thread === */

};

// Testbench module to validate the DUT
SC_MODULE(Testbench) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_out<bool> o_rst;
/* === End Fixed Format: Clock and Reset Signals === */

    /* Testbench Output ports to DUT (FIFO) */
    sc_fifo_out<int> o_a;
    sc_fifo_out<int> o_b;
    /* Testbench Input ports from DUT (FIFO) */
    sc_fifo_in<int> i_result;

/* === Fixed Format: Process Registration === */
    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }
/* === End Fixed Format: Process Registration === */

/* === Main Test Thread === */
    void do_test() {
        int a = 11;
        int b = 9;
        int result;

        o_a.write(a);
        o_b.write(b);
        wait(1, SC_NS);
        result = i_result.read();
        std::cout << "Sum(" << a << ", " << b << ") = " << result << std::endl;
        assert(result == 20);

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};


// sc_main to instantiate and connect modules
#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");
/* === End Fixed Format: Clock and Reset Signals === */   

    Testbench tb("tb");
    Dut dut("dut");

    sc_fifo<int> fifo_a;
    sc_fifo<int> fifo_b;
    sc_fifo<int> fifo_result;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_a(fifo_a);
    tb.o_b(fifo_b);
    tb.i_result(fifo_result);

    dut.i_a(fifo_a);
    dut.i_b(fifo_b);
    dut.o_result(fifo_result);

/* === Fixed Format: sc_start and return === */
    sc_start();
    return 0;
/* === End Fixed Format: sc_start and return === */
}
