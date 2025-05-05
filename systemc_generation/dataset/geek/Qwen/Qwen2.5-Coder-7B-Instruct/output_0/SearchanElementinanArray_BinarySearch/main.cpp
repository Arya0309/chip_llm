
#include <systemc.h>


SC_MODULE(BinarySearchModule) {
    sc_in<int> target;  // Input port for the target value
    sc_out<bool> found; // Output port indicating if target is found
    sc_vector<sc_in<int>> v; // Input vector of integers

    // Constructor
    SC_CTOR(BinarySearchModule) : found("found"), v("v") {
        // Initialize found to false
        found.initialize(false);

        // Process to perform binary search
        SC_METHOD(search);
        sensitive << target;
        for (unsigned i = 0; i < v.size(); ++i) {
            sensitive << v[i];
        }
    }

    // Method to perform binary search
    void search() {
        int low = 0;
        int high = v.size() - 1;
        bool isFound = false;

        if (v.size() == 0) {
            found.write(false);
            return;
        }

        while (low <= high) {
            int mid = ((high - low) / 2) + low;

            if (mid >= v.size()) {
                found.write(false);
                return;
            }

            if (v[mid].read() == target.read()) {
                isFound = true;
                break;
            } else if (v[mid].read() > target.read()) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        found.write(isFound);
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
