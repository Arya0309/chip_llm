c
// Iterative Binary Search in SystemC

#include <systemc.h>

#include <vector>

using namespace sc_core;


SC_MODULE(BinarySearchModule) {
    // Declare the signals used by the module
    sc_in<vector<int>> vector_input;
    sc_in<int> target_input;
    sc_out<bool> found_output;

    // Declare the module's interface
    SC_HAS_PROCESS(BinarySearchModule);

    BinarySearchModule(sc_module_name name) : sc_module(name), vector_input("vector_input"), 
                                         target_input("target_input"), found_output("found_output") {
        // Create a process to perform the binary search
        SC_THREAD(search_process);
        sensitive << vector_input << target_input;
    }

    void search_process() {
        int low = 0, high = vector_input.size() - 1;

        while (low <= high) {
            int mid = (high - low) / 2 + low;
            
            // If the middle element is equal to the target, set
            // found_output to true and exit the loop
            if (vector_input[mid] == target_input) {
                found_output.write(true);
                break;
            }

            // If the middle element is greater than the target,
            // update the high boundary of the search space to the
            // previous midpoint and repeat the loop
            else if(vector_input[mid] > target_input) {
                high = mid - 1;
            }

            // If the middle element is smaller than the target,
            // update the low boundary of the search space to the
            // next midpoint and repeat the loop
            else {
                low = mid + 1;
            }
        }
    }
};



SC_MODULE(Testbench) {
    sc_vector<sc_signal<int>> v_sig; // Signals for the vector elements
    sc_signal<int> target_sig;       // Signal for the target value
    sc_signal<bool> found_sig;       // Signal for the result

    BinarySearchModule bs_inst; // Instance of the BinarySearchModule

    // Constructor
    SC_CTOR(Testbench)
    : bs_inst("bs_inst"), v_sig("v_sig", 8) { // Initialize vector size to 8
        // Connect signals to module ports
        for (size_t i = 0; i < v_sig.size(); ++i) {
            bs_inst.v[i](v_sig[i]);
        }
        bs_inst.target(target_sig);
        bs_inst.found(found_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the vector with sorted values
        std::vector<int> v_values = {1, 2, 3, 4, 5, 8, 9, 11};
        for (size_t i = 0; i < v_values.size(); ++i) {
            v_sig[i].write(v_values[i]);
        }

        // Test case 1: Target found
        target_sig.write(8);
        wait(1, SC_NS); // Wait for the module to process
        assert(found_sig.read() == true);
        cout << "Target " << target_sig.read() << " found: " << found_sig.read() << endl;

        // Test case 2: Target not found
        target_sig.write(7);
        wait(1, SC_NS); // Wait for the module to process
        assert(found_sig.read() == false);
        cout << "Target " << target_sig.read() << " found: " << found_sig.read() << endl;

        // End simulation after successful test
        cout << "All tests passed successfully." << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
