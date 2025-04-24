
#include <systemc.h>


SC_MODULE(MinMax) {
    sc_in<sc_int<32> > arr; // Input port for the array
    sc_in<sc_int<32> > n; // Input port for the size of the array
    sc_out<sc_int<32> > min_val; // Output port for the minimum value
    sc_out<sc_int<32> > max_val; // Output port for the maximum value

    // Constructor
    SC_CTOR(MinMax) {
        // Process to find the minimum and maximum values in the array
        SC_METHOD(findMinMax);
        sensitive << arr << n;
    }

    // Method to find the minimum and maximum values in the array
    void findMinMax() {
        sc_int<32> min = sc_numeric_limits<sc_int<32> >::max();
        sc_int<32> max = sc_numeric_limits<sc_int<32> >::min();
        for (sc_int<32> i = 0; i < n.read(); i++) {
            sc_int<32> current = arr.read(i);
            if (current < min) {
                min = current;
            }
            if (current > max) {
                max = current;
            }
        }
        min_val.write(min);
        max_val.write(max);
    }
};


SC_MODULE(Testbench) {
    sc_clock clk;           // Clock signal for synchronization
    sc_signal<bool> start;  // Signal to trigger the computation
    sc_signal<bool> done;   // Signal indicating computation completion
    sc_signal<int> min_val; // Signal for minimum element output
    sc_signal<int> max_val; // Signal for maximum element output

    // Instance of the MinMax module
    MinMax* minmax_inst;

    SC_CTOR(Testbench)
        : clk("clk", 1, SC_NS) // 1 ns clock period
    {
        minmax_inst = new MinMax("minmax_inst");
        minmax_inst->clk(clk);
        minmax_inst->start(start);
        minmax_inst->done(done);
        minmax_inst->min_val(min_val);
        minmax_inst->max_val(max_val);

        SC_THREAD(run_tests);
    }

    // Process to run the test case
    void run_tests() {
        // Define and load the input array
        int input_arr[5] = {12, 1234, 45, 67, 1};
        minmax_inst->load_array(input_arr);

        // Start the computation by asserting the start signal
        start.write(true);
        wait(1, SC_NS);
        start.write(false);

        // Wait until the MinMax module signals that computation is done
        while (done.read() == false) {
            wait(1, SC_NS);
        }

        // Retrieve computed results
        int computed_min = min_val.read();
        int computed_max = max_val.read();

        // Expected results: minimum = 1, maximum = 1234
        assert(computed_min == 1);
        assert(computed_max == 1234);

        std::cout << "Computed minimum: " << computed_min << std::endl;
        std::cout << "Computed maximum: " << computed_max << std::endl;
        std::cout << "Test passed successfully." << std::endl;

        sc_stop(); // End the simulation
    }
};

// sc_main: Entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create the testbench instance
    sc_start();         // Start the simulation
    return 0;
}
