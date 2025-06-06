#include <systemc.h>

using namespace sc_core;

// DUT module: Octal to Decimal Converter
SC_MODULE(Dut) {

    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_octal;
    sc_fifo_out<int> o_decimal;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int octal = i_octal.read();
            int temp = octal;
            int decimal = 0;
            int base = 1;

            while (temp) {
                int last_digit = temp % 10;
                temp = temp / 10;
                decimal += last_digit * base;
                base = base * 8;
            }

            o_decimal.write(decimal);
            wait();
        }
    }
};


// Testbench module
SC_MODULE(Testbench) {

    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_octal;
    sc_fifo_in<int> i_decimal;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int input = 67;
        int expected = 55; // 6*8 + 7 = 48 + 7 = 55
        int output;

        o_octal.write(input);
        wait(1, SC_NS);

        output = i_decimal.read();
        std::cout << "Octal " << input << " -> Decimal = " << output << std::endl;
        assert(output == expected);

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

    sc_fifo<int> fifo_in;
    sc_fifo<int> fifo_out;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_octal(fifo_in);
    tb.i_decimal(fifo_out);
    dut.i_octal(fifo_in);
    dut.o_decimal(fifo_out);

    sc_start();
    return 0;
}