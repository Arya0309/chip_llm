
#include <systemc.h>


SC_MODULE(MinMax) {
    sc_in<bool> clk;
    sc_in<bool> start;
    sc_out<bool> done;
    sc_out<int> min_val;
    sc_out<int> max_val;

    // Internal storage for the array
    int arr[5];
    int n = 5;

    SC_CTOR(MinMax) {
        SC_METHOD(find_min_max);
        sensitive << start.pos();
        dont_initialize();
    }

    void find_min_max() {
        // Initialize the array with sample data
        arr[0] = 12;
        arr[1] = 1234;
        arr[2] = 45;
        arr[3] = 67;
        arr[4] = 1;

        // Find minimum and maximum elements
        int min_element = arr[0];
        int max_element = arr[0];
        for (int i = 1; i < n; ++i) {
            if (arr[i] < min_element) {
                min_element = arr[i];
            }
            if (arr[i] > max_element) {
                max_element = arr[i];
            }
        }

        // Output the results
        min_val.write(min_element);
        max_val.write(max_element);

        // Signal completion
        done.write(true);
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
