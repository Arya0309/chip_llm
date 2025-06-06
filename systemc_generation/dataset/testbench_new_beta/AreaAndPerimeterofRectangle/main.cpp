#include <systemc.h>

using namespace sc_core;

// DUT module
SC_MODULE(Dut) {

    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_a;
    sc_fifo_in<int> i_b;
    sc_fifo_out<int> o_area;
    sc_fifo_out<int> o_perimeter;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int a = i_a.read();
            int b = i_b.read();

            int area = a * b;
            int perimeter = 2 * (a + b);

            o_area.write(area);
            o_perimeter.write(perimeter);

            wait();
        }
    }
};

// Testbench
SC_MODULE(Testbench) {

    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_a;
    sc_fifo_out<int> o_b;
    sc_fifo_in<int> i_area;
    sc_fifo_in<int> i_perimeter;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int a = 5, b = 6;
        o_a.write(a);
        o_b.write(b);

        wait(1, SC_NS);

        int area = i_area.read();
        int perimeter = i_perimeter.read();

        std::cout << "Area = " << area << std::endl;
        std::cout << "Perimeter = " << perimeter << std::endl;

        assert(area == 30);
        assert(perimeter == 22);

        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");

    Dut dut("dut");
    Testbench tb("tb");

    sc_fifo<int> fifo_a;
    sc_fifo<int> fifo_b;
    sc_fifo<int> fifo_area;
    sc_fifo<int> fifo_perimeter;

    dut.i_clk(clk);
    dut.i_rst(rst);
    tb.i_clk(clk);
    tb.o_rst(rst);

    dut.i_a(fifo_a);
    dut.i_b(fifo_b);
    dut.o_area(fifo_area);
    dut.o_perimeter(fifo_perimeter);

    tb.o_a(fifo_a);
    tb.o_b(fifo_b);
    tb.i_area(fifo_area);
    tb.i_perimeter(fifo_perimeter);

    sc_start();
    return 0;
} 