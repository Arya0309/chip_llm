
#include <systemc.h>


SC_MODULE(MinMax) {
    sc_in<bool> clk;          // Clock input
    sc_in<bool> start;        // Start signal
    sc_out<bool> done;        // Done signal
    sc_out<int> min_val;     // Minimum value output
    sc_out<int> max_val;     // Maximum value output

    int arr[5];               // Array to store input values
    bool processing;         // Flag to track if processing is ongoing

    SC_CTOR(MinMax) {
        SC_THREAD(process);
        sensitive << clk.pos();
    }

    void process() {
        while (true) {
            wait(); // Wait for a clock cycle
            if (start.read() && !processing) {
                processing = true;
                min_val.write(getMin(arr, 5));
                max_val.write(getMax(arr, 5));
                done.write(true);
                wait(); // Wait for one cycle to signal completion
                done.write(false);
                processing = false;
            }
        }
    }

    int getMin(int arr[], int n) {
        return *min_element(arr, arr + n);
    }

    int getMax(int arr[], int n) {
        return *max_element(arr, arr + n);
    }

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
