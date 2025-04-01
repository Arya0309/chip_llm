
#include <systemc.h>
#include <algorithm>
#include <vector>

// Module to find common elements between two arrays
SC_MODULE(CommonElementsFinder) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_out<bool> done; // Done signal indicating completion

    // Arrays to hold input data
    int arr1[6];
    int arr2[6];

    // Vector to store result
    std::vector<int> v;

    // Constructor
    SC_CTOR(CommonElementsFinder) {
        SC_METHOD(find_common_elements);
        sensitive << clk.posedge_event() << rst;
        dont_initialize();
    }

    // Method to find common elements
    void find_common_elements() {
        if (rst) {
            done.write(false);
            v.clear();
        } else {
            // Sort the arrays
            std::sort(arr1, arr1 + 6);
            std::sort(arr2, arr2 + 6);

            // Find intersection
            std::set_intersection(arr1, arr1 + 6, arr2, arr2 + 6, std::back_inserter(v));

            // Set done signal
            done.write(true);
        }
    }

    // Method to print results
    void print_results() {
        for (auto i: v) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk; // Clock signal
    sc_signal<bool> rst; // Reset signal
    sc_signal<bool> done; // Done signal

    CommonElementsFinder finder_inst; // Instance of the CommonElementsFinder module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), finder_inst("finder") {
        // Connect signals to ports
        finder_inst.clk(clk);
        finder_inst.rst(rst);
        finder_inst.done(done);

        // Initialize arrays
        std::copy({1, 45, 54, 71, 76, 12}, {1, 45, 54, 71, 76, 12} + 6, finder_inst.arr1);
        std::copy({1, 7, 5, 4, 6, 12}, {1, 7, 5, 4, 6, 12} + 6, finder_inst.arr2);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        rst = true;
        wait(1, SC_NS);
        rst = false;
        wait(1, SC_NS);

        // Wait until the operation is done
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print the results
        finder_inst.print_results();

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
