c++
#include <systemc.h>


SC_MODULE(BinarySearchModule) {
    sc_vector<sc_in<int>> v; // Input vector
    sc_in<int> target; // Input target value
    sc_out<bool> found; // Output found indicator

    // Constructor
    SC_CTOR(BinarySearchModule) {
        // Define lower and upper bounds of the search range
        int low = 0, high = v.size() - 1;
        // Define the midpoint of the search range
        int mid = ((high - low) / 2) + low;

        // While loop to search for the target
        while (low <= high) {
            // Check if the middle element is equal to the target
            if (v[mid] == target) {
                // Found the target, set the found indicator to true and exit the loop
                found.write(true);
                break;
            }

            // Check if the middle element is greater than the target
            if (v[mid] > target) {
                // Search the left half of the vector
                high = mid - 1;
            } else {
                // Search the right half of the vector
                low = mid + 1;
            }
        }

        if (low > high) {
            // The target was not found, set the found indicator to false
            found.write(false);
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
