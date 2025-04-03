#include <systemc.h>
#include <assert.h>
#include <iostream>

// Module: MinMax
// Computes the minimum and maximum elements of an internal fixed-size array.
SC_MODULE(MinMax) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start computation
    sc_out<bool> done;    // Signal to indicate computation completion
    sc_out<int> min_val;  // Output: minimum element
    sc_out<int> max_val;  // Output: maximum element

    // Internal storage for the array (fixed size of 5 elements)
    static const int SIZE = 5;
    int arr[SIZE];

    // Constructor: Registers the compute_process with clock sensitivity.
    SC_CTOR(MinMax) {
        SC_THREAD(compute_process);
        sensitive << clk.pos();
    }

    // Helper method to load an input array into the module
    void load_array(const int input[SIZE]) {
        for (int i = 0; i < SIZE; i++) {
            arr[i] = input[i];
        }
    }

    // Process that waits for the start signal and performs the computation
    void compute_process() {
        while (true) {
            wait(); // Wait for the next clock cycle
            if (start.read() == true) {
                // Initialize local variables with the first array element
                int local_min = arr[0];
                int local_max = arr[0];

                // Loop over the array to determine min and max
                for (int i = 1; i < SIZE; i++) {
                    if (arr[i] < local_min)
                        local_min = arr[i];
                    if (arr[i] > local_max)
                        local_max = arr[i];
                }

                // Write the computed values to the output ports
                min_val.write(local_min);
                max_val.write(local_max);
                done.write(true);

                // Wait one cycle to signal completion then reset 'done'
                wait();
                done.write(false);
            }
        }
    }
};

// Testbench Module: Sets up inputs, triggers computation, and verifies output
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
