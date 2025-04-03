
#include <systemc.h>

// Binary Search Module
SC_MODULE(BinarySearchModule) {
    sc_vector<sc_in<int>> input_array; // Input array elements
    sc_in<int> target;                 // Target element to search
    sc_out<bool> result;               // Result of the search (found or not)

    int size; // Size of the input array

    // Constructor
    SC_CTOR(BinarySearchModule, const char* name, int array_size) : sc_module(name), size(array_size) {
        // Register the search process
        SC_METHOD(binary_search);
        sensitive << target;
    }

    // Binary search method
    void binary_search() {
        int low = 0, high = size - 1;
        bool found = false;

        while (low <= high && !found) {
            int mid = ((high - low) / 2) + low;
            int mid_value = input_array[mid].read();

            if (mid_value == target.read()) {
                found = true;
            } else if (mid_value > target.read()) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        result.write(found);
    }
};

// Testbench Module
SC_MODULE(Testbench) {
    sc_signal<int> target;             // Target element to search
    sc_signal<bool> result;            // Result of the search (found or not)

    BinarySearchModule bs_module;     // Instance of the BinarySearchModule

    // Constructor
    SC_CTOR(Testbench) : bs_module("bs_module", 8) {
        // Connect signals to ports
        for (int i = 0; i < 8; ++i) {
            sc_signal<int> element;
            element.write(i + 1); // Initialize with values 1 to 8
            bs_module.input_array.push_back(element);
        }
        bs_module.target(target);
        bs_module.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Target found
        target = 8;
        wait(1, SC_NS);
        assert(result.read() == true);
        cout << "Test 1: Target " << target.read() << (result.read() ? " found." : " NOT found.") << endl;

        // Test case 2: Target not found
        target = 10;
        wait(1, SC_NS);
        assert(result.read() == false);
        cout << "Test 2: Target " << target.read() << (result.read() ? " found." : " NOT found.") << endl;

        // Test case 3: Target at the start
        target = 1;
        wait(1, SC_NS);
        assert(result.read() == true);
        cout << "Test 3: Target " << target.read() << (result.read() ? " found." : " NOT found.") << endl;

        // Test case 4: Target at the end
        target = 11;
        wait(1, SC_NS);
        assert(result.read() == false);
        cout << "Test 4: Target " << target.read() << (result.read() ? " found." : " NOT found.") << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
