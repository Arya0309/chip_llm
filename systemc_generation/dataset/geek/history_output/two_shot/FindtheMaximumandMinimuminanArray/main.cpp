
#include <systemc.h>

// Module to find minimum and maximum elements in an array
SC_MODULE(MinMaxFinder) {
    sc_in<bool> clk;            // Clock signal
    sc_in<bool> rst;            // Reset signal
    sc_in<sc_uint<32>> data_in; // Input data
    sc_in<bool> valid_in;       // Valid signal indicating new data
    sc_out<sc_uint<32>> min_out; // Minimum element output
    sc_out<sc_uint<32>> max_out; // Maximum element output
    sc_out<bool> ready_out;     // Ready signal indicating processing completion

    // Internal variables
    sc_uint<32> min_val;
    sc_uint<32> max_val;
    bool ready;

    // Constructor
    SC_CTOR(MinMaxFinder) {
        // Initialize internal variables
        min_val = 0xFFFFFFFF; // Set to maximum possible value
        max_val = 0x00000000; // Set to minimum possible value
        ready = false;

        // Register the method to process data
        SC_METHOD(process_data);
        sensitive << clk.posedge_event() << rst.posedge_event();
    }

    // Method to process incoming data
    void process_data() {
        if (rst.read()) {
            // Reset state
            min_val = 0xFFFFFFFF;
            max_val = 0x00000000;
            ready = false;
        } else if (valid_in.read()) {
            // Update min and max values
            if (data_in.read() < min_val) {
                min_val = data_in.read();
            }
            if (data_in.read() > max_val) {
                max_val = data_in.read();
            }
            ready = true;
        }
        // Drive outputs
        min_out.write(min_val);
        max_out.write(max_val);
        ready_out.write(ready);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;              // Clock signal
    sc_signal<bool> rst;       // Reset signal
    sc_signal<sc_uint<32>> data_in; // Input data
    sc_signal<bool> valid_in;   // Valid signal indicating new data
    sc_signal<sc_uint<32>> min_out; // Minimum element output
    sc_signal<sc_uint<32>> max_out; // Maximum element output
    sc_signal<bool> ready_out;  // Ready signal indicating processing completion

    MinMaxFinder min_max_finder_inst; // Instance of the MinMaxFinder module

    // Constructor
    SC_CTOR(Testbench) : min_max_finder_inst("min_max_finder") {
        // Connect signals to module ports
        min_max_finder_inst.clk(clk);
        min_max_finder_inst.rst(rst);
        min_max_finder_inst.data_in(data_in);
        min_max_finder_inst.valid_in(valid_in);
        min_max_finder_inst.min_out(min_out);
        min_max_finder_inst.max_out(max_out);
        min_max_finder_inst.ready_out(ready_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        rst.write(true);
        wait(2, SC_NS);
        rst.write(false);
        wait(1, SC_NS);

        // Test case 1: Array [12, 1234, 45, 67, 1]
        sc_uint<32> test_array[] = {12, 1234, 45, 67, 1};
        for (int i = 0; i < 5; ++i) {
            data_in.write(test_array[i]);
            valid_in.write(true);
            wait(1, SC_NS);
            valid_in.write(false);
            wait(1, SC_NS);
        }
        wait(1, SC_NS); // Ensure final values are processed
        cout << "Minimum element of array: " << min_out.read() << " ";
        cout << "Maximum element of array: " << max_out.read() << endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
