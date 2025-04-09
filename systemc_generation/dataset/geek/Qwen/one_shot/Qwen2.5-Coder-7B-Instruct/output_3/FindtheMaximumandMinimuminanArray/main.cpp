
#include <systemc.h>


SC_MODULE(MinMax) {
    // Ports
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> start;        // Start signal
    sc_out<bool> done;        // Done signal
    sc_out<int> min_val;      // Minimum value output
    sc_out<int> max_val;      // Maximum value output

    // Internal signals
    sc_signal<int> arr[5];    // Array of integers
    sc_signal<int> n;         // Size of the array
    sc_signal<int> local_min; // Local minimum value
    sc_signal<int> local_max; // Local maximum value

    // Constructor
    SC_CTOR(MinMax) {
        // Process to handle the computation
        SC_METHOD(compute);
        sensitive << clk.pos();
        dont_initialize();

        // Initialize the array values
        arr[0] = 12;
        arr[1] = 1234;
        arr[2] = 45;
        arr[3] = 67;
        arr[4] = 1;

        // Set the size of the array
        n = 5;
    }

    // Method to compute the minimum and maximum values
    void compute() {
        if (start.read()) {
            local_min = arr[0];
            local_max = arr[0];

            for (int i = 1; i < n.read(); ++i) {
                if (arr[i].read() < local_min.read())
                    local_min = arr[i];
                if (arr[i].read() > local_max.read())
                    local_max = arr[i];
            }

            // Assign the results to output ports
            min_val.write(local_min);
            max_val.write(local_max);

            // Signal completion
            done.write(true);
        } else {
            done.write(false);
        }
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
