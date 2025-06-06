#include <systemc.h>

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

    sc_in_clk i_clk;
    sc_in<bool> i_rst;

    sc_fifo_in<int> i_val;
    sc_fifo_out<bool> o_result;

    SC_CTOR(Dut) {
        SC_THREAD(do_compute);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_compute() {
        while (true) {
            int n = i_val.read();
            int temp = n;
            int sum = 0;
            int digits = 0;

            // Count number of digits
            int count_temp = temp;
            while (count_temp > 0) {
                count_temp /= 10;
                digits++;
            }

            // Calculate sum of each digit raised to the power of digits
            count_temp = temp;
            while (count_temp > 0) {
                int rem = count_temp % 10;
                int power = 1;
                for (int i = 0; i < digits; i++) {
                    power *= rem;
                }
                sum += power;
                count_temp /= 10;
            }

            bool is_armstrong = (sum == temp);
            o_result.write(is_armstrong);
            wait();
        }
    }

};

// Testbench module
SC_MODULE(Testbench) {

    sc_in_clk i_clk;
    sc_out<bool> o_rst;

    sc_fifo_out<int> o_val;
    sc_fifo_in<bool> i_result;

    SC_CTOR(Testbench) {
        SC_THREAD(do_test);
        sensitive << i_clk.pos();
        dont_initialize();
    }

    void do_test() {
        int test_values[] = {153, 370, 9474, 9475};
        bool expected[] = {true, true, true, false};

        for (int i = 0; i < 4; i++) {
            o_val.write(test_values[i]);
            wait(1, SC_NS);
            bool result = i_result.read();
            std::cout << "Value: " << test_values[i] << ", Armstrong: " << std::boolalpha << result << std::endl;
            assert(result == expected[i]);
        }

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

    sc_fifo<int> fifo_val;
    sc_fifo<bool> fifo_result;

    tb.i_clk(clk);
    tb.o_rst(rst);
    dut.i_clk(clk);
    dut.i_rst(rst);

    tb.o_val(fifo_val);
    tb.i_result(fifo_result);

    dut.i_val(fifo_val);
    dut.o_result(fifo_result);

    sc_start();
    return 0;
}
