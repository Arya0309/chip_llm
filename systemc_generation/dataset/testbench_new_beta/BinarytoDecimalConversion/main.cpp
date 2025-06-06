#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {
    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_binary;
    sc_fifo_out<int> o_decimal;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int bin = i_binary.read();
            int dec = 0;
            int base = 1;

            int temp = bin;
            while (temp) {
                int last_digit = temp % 10;
                temp = temp / 10;
                dec += last_digit * base;
                base *= 2;
            }

            o_decimal.write(dec);
            wait();
        }
    }
};

SC_MODULE(Testbench) {
    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_binary;
    sc_fifo_in<int> i_decimal;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int input_bin = 10101001; // binary input
        int expected_dec = 169;   // expected decimal output

        o_binary.write(input_bin);
        wait(1, SC_NS);

        int output = i_decimal.read();
        std::cout << "binary: " << input_bin << " -> decimal: " << output << std::endl;
        assert(output == expected_dec);

        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");

    Dut dut("dut");
    Testbench tb("tb");

    sc_fifo<int> fifo_binary;
    sc_fifo<int> fifo_decimal;

    dut.i_clk(clk);
    dut.i_rst(rst);
    dut.i_binary(fifo_binary);
    dut.o_decimal(fifo_decimal);

    tb.i_clk(clk);
    tb.o_rst(rst);
    tb.o_binary(fifo_binary);
    tb.i_decimal(fifo_decimal);

    sc_start();
    return 0;
}
