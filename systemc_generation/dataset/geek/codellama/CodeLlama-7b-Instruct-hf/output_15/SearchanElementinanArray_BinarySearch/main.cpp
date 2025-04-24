
#include <systemc.h>

// Module that performs iterative binary search

SC_MODULE(BinarySearchModule) {
    // Input port for the vector
    sc_in<int> vec[];
    // Input port for the target
    sc_in<int> target;
    // Output port indicating whether the target is found
    sc_out<bool> found;

    // Constructor
    SC_CTOR(BinarySearchModule) {
        // Define the lower and upper boundaries
        // of the search interval
        m_bounaries = sc_bounaries(0, vec.size());
    }

    // Process to perform the search
    SC_METHOD(search);
    sensitive << m_bounaries;

    // Function that performs the search
    void search() {
        // Define the current search interval
        int low = m_bounaries.low();
        int high = m_bounaries.high();

        // While the search interval is not empty
        while (!(low > high)) {
            // Compute the midpoint of the interval
            int mid = (low + high) / 2;

            // Check if the middle element equals the target
            if (vec[mid] == target) {
                // Update the output port
                found.write(true);

                // Break out of the loop
                break;
            }

            // Check if the middle element is greater than the target
            if (vec[mid] > target) {
                // Update the upper search boundary
                high = mid - 1;
            }
            else {
                // Update the lower search boundary
                low = mid + 1;
            }
        }

        // If we didn't find the target
        if (found == false) {
            // Update the output port
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
