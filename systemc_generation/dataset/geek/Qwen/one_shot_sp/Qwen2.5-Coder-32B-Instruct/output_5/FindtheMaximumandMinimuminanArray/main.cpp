
#include <systemc.h>

// MinMax Module: Finds the minimum and maximum elements in an array

SC_MODULE(MinMax) {
    sc_in<bool> clk;      // Clock input
    sc_in<bool> start;    // Signal to start processing
    sc_out<bool> done;    // Signal to indicate processing is complete
    sc_out<int> min_val;  // Output for minimum value
    sc_out<int> max_val;  // Output for maximum value

    // Internal storage for the array (fixed size of 5 elements)
    int arr[5];

    // Constructor: register the process
    SC_CTOR(MinMax) {
        SC_THREAD(process);
        sensitive << clk.pos();
    }

    // Process that waits for the start signal and computes min/max
    void process() {
        while (true) {
            wait(); // wait for a clock cycle
            if (start.read() == true) {
                computeMinMax(); // compute min and max
                done.write(true);
                wait(); // wait for one cycle to signal completion
                done.write(false);
            }
        }
    }

    // Function to compute the minimum and maximum elements in the array
    void computeMinMax() {
        // Number of elements is fixed at 5
        const int n = 5;
        int min_value = arr[0];
        int max_value = arr[0];
        
        for (int i = 1; i < n; i++) {
            if (arr[i] < min_value) {
                min_value = arr[i];
            }
            if (arr[i] > max_value) {
                max_value = arr[i];
            }
        }
        
        min_val.write(min_value);
        max_val.write(max_value);
    }

    // Helper method to load an input array into the module
    void load_array(const int input[5]) {
        for (int i = 0; i < 5; i++) {
            arr[i] = input[i];
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
