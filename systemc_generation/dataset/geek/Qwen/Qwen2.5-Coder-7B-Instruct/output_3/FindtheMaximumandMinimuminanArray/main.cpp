
#include <systemc.h>


SC_MODULE(MinMax) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> start;  // Start signal
    sc_out<bool> done;  // Done signal
    sc_out<int> min_val;  // Output port for the minimum value
    sc_out<int> max_val;  // Output port for the maximum value

    sc_in<sc_vector<int>> input_arr;  // Input port for the array
    sc_signal<int> min_result;  // Internal signal for the minimum result
    sc_signal<int> max_result;  // Internal signal for the maximum result

    // Constructor
    SC_CTOR(MinMax) {
        // Process to find minimum and maximum values
        SC_METHOD(find_min_max);
        sensitive << clk.pos();
        dont_initialize();

        // Process to handle the clock edge
        SC_METHOD(handle_clock);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to find minimum and maximum values
    void find_min_max() {
        if (start.read()) {
            if (!input_arr.empty()) {
                int min_val_temp = *std::min_element(input_arr.begin(), input_arr.end());
                int max_val_temp = *std::max_element(input_arr.begin(), input_arr.end());

                // Check if the values are within a safe range
                if (min_val_temp >= INT_MIN && min_val_temp <= INT_MAX) {
                    min_result = min_val_temp;
                } else {
                    min_result = INT_MIN;  // Handle overflow or underflow case
                }

                if (max_val_temp >= INT_MIN && max_val_temp <= INT_MAX) {
                    max_result = max_val_temp;
                } else {
                    max_result = INT_MAX;  // Handle overflow or underflow case
                }
            } else {
                min_result = 0;  // Default value if array is empty
                max_result = 0;  // Default value if array is empty
            }
            done.write(true);
        } else {
            done.write(false);
        }
    }

    // Method to update the output ports
    void update_outputs() {
        if (min_result > INT_MIN && max_result < INT_MAX) {
            min_val.write(min_result);
            max_val.write(max_result);
        } else {
            // Handle overflow or underflow case
            min_val.write(0);  // Default value in case of overflow
            max_val.write(0);  // Default value in case of overflow
        }
    }

    // Process to handle the clock edge
    void handle_clock() {
        if (done.read()) {
            update_outputs();
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
