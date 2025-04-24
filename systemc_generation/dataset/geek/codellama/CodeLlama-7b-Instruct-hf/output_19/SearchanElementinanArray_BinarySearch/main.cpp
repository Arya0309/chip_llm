c
#include <systemc.h>

// Module that implements iterative binary search

SC_MODULE(BinarySearchModule) {
    sc_in<sc_int> v; // Input port for the vector of integers
    sc_in<sc_int> target; // Input port for the target integer
    sc_out<sc_bool> found; // Output port for the search results

    // Process to perform binary search
    SC_METHOD(search);
    sensitive << v << target;

    // Function to search the vector for the target integer
    sc_bool search() {
        // Define initial low and high indices
        sc_int low = 0, high = v.size() - 1;

        // Loop until the target integer is found or the vector cannot be divided any further
        while (low <= high) {
            // Calculate the midpoint
            sc_int mid = (high + low) / 2;

            // If the middle element is equal to the target, return true
            if (v[mid] == target)
                return true;

            // If the middle element is greater than the target, search the left half
            if (v[mid] > target)
                high = mid - 1;

            // If the middle element is less than the target, search the right half
            else
                low = mid + 1;
        }

        // If the target integer is not found, return false
        return false;
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
