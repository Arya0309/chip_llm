#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_n;
    sc_fifo_out<int> o_sum;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int n = i_n.read();
            int sum = 0;
            for (int i = 1; i <= n; ++i) {
                sum += i;
            }
            o_sum.write(sum);
            wait();
        }
    }
};


SC_MODULE(Testbench) {

    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_n;
    sc_fifo_in<int> i_sum;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int n = 5;
        int expected_sum = 15;

        o_n.write(n);
        wait(1, SC_NS);
        int result = i_sum.read();

        std::cout << "Sum of first " << n << " natural numbers = " << result << std::endl;
        assert(result == expected_sum);

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

    sc_fifo<int> fifo_n;
    sc_fifo<int> fifo_sum;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_n(fifo_n);
    tb.i_sum(fifo_sum);

    dut.i_n(fifo_n);
    dut.o_sum(fifo_sum);

    sc_start();
    return 0;
}
