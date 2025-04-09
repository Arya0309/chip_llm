
#include <systemc.h>

// Define a SystemC module for Binary Search

SC_MODULE(BinarySearchModule) {
    // Input port for the target value
    sc_in<int> target;
    // Output port to indicate if the target is found
    sc_out<bool> found;
    // Vector of input ports representing the sorted array
    sc_vector<sc_in<int>> v;

    // Constructor for the module
    SC_CTOR(BinarySearchModule) : v("v", N) {
        // Sensitivity list: react to changes in target or any element in v
        SC_METHOD(binary_search_method);
        sensitive << target;
        for (int i = 0; i < N; ++i) {
            sensitive << v[i];
        }
    }

    // Method implementing the binary search logic
    void binary_search_method() {
        int low = 0, high = N - 1;
        bool result = false;

        while (low <= high) {
            int mid = ((high - low) / 2) + low;
            int mid_value = v[mid].read();

            if (mid_value == target.read()) {
                result = true;
                break;
            } else if (mid_value > target.read()) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        found.write(result);
    }

private:
    static const int N = 8; // Size of the vector
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
