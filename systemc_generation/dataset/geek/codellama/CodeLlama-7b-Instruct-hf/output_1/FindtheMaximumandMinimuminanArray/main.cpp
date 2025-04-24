
#include <systemc.h>

// Module that finds the minimum and maximum elements 
// in an integer array.

SC_MODULE(MinMax) {
    // Input ports for the array data
    sc_in<int> arr[10]; // arrays of integers, with length = 10

    // Output ports for the minimum and maximum elements
    sc_out<int> min; // output port for the minimum element
    sc_out<int> max; // output port for the maximum element

    // Constructor
    SC_CTOR(MinMax) {
    }

    // Boundary condition:
    // - if length of the input array is 0, output ports
    //   min and max should return 0
    int len;
    sc_signal<int> bound;
    void boundary_conditions(int len, int bound) {
        if (len == 0) {
            min.write(0);
            max.write(0);
        }
    }

    // Find minimum and maximum elements, and write
    // them to the output ports min and max
    int min_val, max_val;
    void find_min_max(int min_val, int max_val) {
        // array of integers, with length = 10
        int arr[10];
        // Get the length of the input array
        len = sizeof(arr) / sizeof(arr[0]);
        // If length of the input array is 0, skip
        // the rest of the code
        if (len == 0) {
            return;
        }
        // Get the minimum and maximum elements
        // in the input array
        min_val = *min_element(arr, arr + len);
        max_val = *max_element(arr, arr + len);
        // Write the minimum and maximum elements
        // to the output ports
        min.write(min_val);
        max.write(max_val);
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
