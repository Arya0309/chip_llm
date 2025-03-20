
#include <systemc.h>
#include <vector>
#include <algorithm>

// Module to find common elements between two arrays
SC_MODULE(CommonElementsFinder) {
    sc_in<bool> clk;             // Clock signal
    sc_in<bool> rst;             // Reset signal
    sc_in<sc_uint<32>> start;     // Start signal to initiate the process
    sc_out<sc_uint<32>> done;     // Done signal indicating completion
    sc_out<sc_uint<32>> result;   // Output port for the result (common element)

    // Internal signals and variables
    sc_uint<32> arr1[6] = {1, 45, 54, 71, 76, 12}; // First array
    sc_uint<32> arr2[6] = {1, 7, 5, 4, 6, 12};     // Second array
    std::vector<sc_uint<32>> v;                    // Vector to store common elements
    int index;                                     // Index for iterating through results

    // Constructor
    SC_CTOR(CommonElementsFinder) {
        // Process to find common elements
        SC_METHOD(find_common_elements);
        sensitive << clk.posedge_event() << rst;
        dont_initialize();

        // Process to send results
        SC_METHOD(send_results);
        sensitive << clk.posedge_event() << rst;
        dont_initialize();
    }

    // Method to find common elements
    void find_common_elements() {
        if (rst) {
            v.clear();
            done.write(0);
            index = 0;
        } else if (start) {
            // Sort the arrays
            std::sort(arr1, arr1 + 6);
            std::sort(arr2, arr2 + 6);

            // Find common elements
            std::set_intersection(arr1, arr1 + 6, arr2, arr2 + 6, std::back_inserter(v));
            done.write(1);
        }
    }

    // Method to send results
    void send_results() {
        if (done && index < v.size()) {
            result.write(v[index]);
            index++;
        } else {
            result.write(0); // No more results to send
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;          // Clock signal
    sc_signal<bool> rst;   // Reset signal
    sc_signal<sc_uint<32>> start; // Start signal
    sc_signal<sc_uint<32>> done;  // Done signal
    sc_signal<sc_uint<32>> result; // Result signal

    CommonElementsFinder finder_inst; // Instance of the CommonElementsFinder module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS, 0.5), finder_inst("finder_inst") {
        // Connect signals to ports
        finder_inst.clk(clk);
        finder_inst.rst(rst);
        finder_inst.start(start);
        finder_inst.done(done);
        finder_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        rst = 1;
        start = 0;
        wait(2, SC_NS);
        rst = 0;
        wait(2, SC_NS);

        // Start the process
        start = 1;
        wait(2, SC_NS);
        start = 0;

        // Wait until the process is done
        while (!done) {
            wait(2, SC_NS);
        }

        // Print the results
        std::cout << "Common elements: ";
        while (result != 0) {
            std::cout << result << " ";
            wait(2, SC_NS);
        }
        std::cout << std::endl;

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
