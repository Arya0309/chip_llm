#include <systemc.h>

using namespace sc_core;

// DUT module to compute nth Fibonacci number
SC_MODULE(Dut) {
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_n;
    sc_fifo_out<int> o_result;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int n = i_n.read();
            int curr = 0;

            if (n <= 1) {
                curr = n;
            } else {
                int prev1 = 1, prev2 = 0;
                for (int i = 2; i <= n; ++i) {
                    curr = prev1 + prev2;
                    prev2 = prev1;
                    prev1 = curr;
                }
            }

            o_result.write(curr);
            wait();
        }
    }
};


// Testbench module
SC_MODULE(Testbench) {
    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_n;
    sc_fifo_in<int> i_result;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int n = 5;
        int result;

        o_n.write(n);
        wait(1, SC_NS);
        result = i_result.read();

        std::cout << "Fibonacci(" << n << ") = " << result << std::endl;
        assert(result == 5);

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

    sc_fifo<int> fifo_n;
    sc_fifo<int> fifo_result;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_n(fifo_n);
    tb.i_result(fifo_result);

    dut.i_n(fifo_n);
    dut.o_result(fifo_result);

    sc_start();
    return 0;
}
