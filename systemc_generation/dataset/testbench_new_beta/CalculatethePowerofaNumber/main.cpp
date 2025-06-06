#include <systemc.h>
#include <cmath> // for pow

using namespace sc_core;

// DUT (Design Under Test) module
SC_MODULE(Dut) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_in_clk i_clk;
    sc_in<bool> i_rst;
/* === End Fixed Format: Clock and Reset Signals === */

    /* LLM should generate correct Dut Input ports (FIFO) */
    sc_fifo_in<double> i_a;
    sc_fifo_in<double> i_b;
    /* LLM should generate correct Dut Output ports (FIFO) */
    sc_fifo_out<double> o_result;

/* === Fixed Format: Process Registration === */
    // SC_HAS_PROCESS is optional in SystemC 2.3.0+
    // SC_HAS_PROCESS(Dut); 
    
    // Constructor
    SC_CTOR(Dut) {
        SC_THREAD(do_compute);           // Register the compute thread
        sensitive << i_clk.pos();        // Trigger on positive clock edge
        dont_initialize();               // Do not trigger at time 0
    }
/* === End Fixed Format: Process Registration === */

/* === Main Compute Thread === */
    void do_compute() {

        /* === Fixed while loop === */
        while (true) {

            /* LLM should generate correct dut function */
            double x = i_a.read();
            double y = i_b.read();
            double res = pow(x, y);
            o_result.write(res);

            wait();         // Wait for next clock cycle

            /* LLM generate dut function end */
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

    /* LLM should generate correct Testbench Output ports to DUT (FIFO)*/
    sc_fifo_out<double> o_a;
    sc_fifo_out<double> o_b;
    /* LLM should generate correct Testbench Input ports to DUT (FIFO)*/
    sc_fifo_in<double> i_result;

/* === Fixed Format: Process Registration === */
    // SC_HAS_PROCESS(Testbench);
    // Constructor
    SC_CTOR(Testbench) {
        SC_THREAD(do_test);              // Register the test thread
        sensitive << i_clk.pos();        // Trigger on positive clock edge
        dont_initialize();               // Do not trigger at time 0
    }
/* === End Fixed Format: Process Registration === */

/* === Main Test Thread === */
    void do_test() {

        /* LLM should generate correct Testbench function */
        double a, b;
        double result;

        // Test case: a = 6.1, b = 4.8
        a = 6.1;
        b = 4.8;
        o_a.write(a);
        o_b.write(b);
        wait(1, SC_NS); // Wait for the process to update
        result = i_result.read();
        std::cout << a << "^" << b << " = " << result << std::endl;

        /* LLM generate Testbench function end */

/* === Fixed Format: sc_stop === */
        std::cout << "Test completed successfully." << std::endl;
        sc_stop();      // End simulation
/* === End Fixed Format: sc_stop === */
    }
};


// Testbench module to validate the DUT
#define CLOCK_PERIOD 1.0
int sc_main(int argc, char* argv[]) {

/* === Fixed Format: Clock and Reset Signals === */
    sc_clock clk("clk", CLOCK_PERIOD, SC_NS);
    sc_signal<bool> rst("rst");
/* === End Fixed Format: Clock and Reset Signals === */   

/* === Fixed Format: Create modules === */
    Testbench tb("tb"); 
    Dut dut("dut");
/* === End Fixed Format: Create modules === */   

    /* LLM should create correct FIFO channels */
    sc_fifo<double> fifo_a;
    sc_fifo<double> fifo_b;
    sc_fifo<double> fifo_result;

    /* LLM should connect FIFO channels with modules correctly */
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

    /* LLM create and connect FIFO  end */

/* === Fixed Format: sc_start and return === */
    sc_start();         // Start the simulation
    return 0;
/* === End Fixed Format: sc_start and return === */
}
