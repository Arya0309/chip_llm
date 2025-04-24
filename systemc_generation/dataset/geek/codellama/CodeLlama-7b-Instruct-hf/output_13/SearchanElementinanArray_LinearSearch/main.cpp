
#include <systemc.h>

// LinearSearch module

SC_MODULE(LinearSearch) {
    // Vector of integers
    sc_in<sc_vector<int>> v;
    // Key to search for
    sc_in<int> key;
    // Output index of first occurrence of key, or -1 if key not found
    sc_out<int>` index;

    // Process to perform linear search
    SC_METHOD(performLinearSearch);
    sensitive << v << key;

    // Method to perform linear search
    void performLinearSearch() {
        // Start from the beginning of the vector
        int i = 0;

        // Keep iterating until we find the key or reach the end of the vector
        while (i < v.size() && v[i] != key) {
            // Increment i to go to the next element
            i++;
        }

        // If we found the key, return the index
        if (i < v.size()) {
            index.write(i + 1);
        }
        // Otherwise, return -1 to indicate key not found
        else {
            index.write(-1);
        }
    }
};

int main() {
    // Testbench
    vector<int> v = {1, 2, 3, 4, 5, 8, 9, 11};
    int key = 8;

    // Create a LinearSearch module
    LinearSearch LinearSearch_module(v, key);

    // Connect the inputs and outputs of the module
    LinearSearch_module.v(v);
    LinearSearch_module.key(key);
    LinearSearch_module.index(index);

    // Start the simulation
    sc_start();

    // Print the output of the simulation
    std::cout << "The index of the first occurrence of " << key << " in the vector is " << index << "\n";

    // End the simulation
    sc_stop();
    return 0;
}


SC_MODULE(Testbench) {
    sc_signal<int> key_sig;       // Signal for the search key
    sc_signal<int> index_sig;     // Signal for the search result index
    sc_vector< sc_signal<int> > data_signals; // Vector of signals for the data array

    LinearSearch* ls_inst;        // Instance of the LinearSearch module

    SC_CTOR(Testbench) : data_signals("data_signals", 8) {
        // Instantiate LinearSearch with an array size of 8
        ls_inst = new LinearSearch("ls_inst", 8);
        ls_inst->key(key_sig);
        ls_inst->index(index_sig);
        for (int i = 0; i < 8; i++) {
            ls_inst->data[i](data_signals[i]);
        }

        // Initialize the data array in the testbench (no test data inside LinearSearch)
        data_signals[0].write(1);
        data_signals[1].write(2);
        data_signals[2].write(3);
        data_signals[3].write(4);
        data_signals[4].write(5);
        data_signals[5].write(8);
        data_signals[6].write(9);
        data_signals[7].write(11);

        SC_THREAD(run_tests);
    }

    // Thread that applies test cases and checks the outputs
    void run_tests() {
        // Test Case 1: key = 8; expected index = 5
        key_sig.write(8);
        wait(1, SC_NS);
        assert(index_sig.read() == 5);
        std::cout << "Test 1 Passed: 8 found at index " << index_sig.read() << std::endl;

        // Test Case 2: key = 10; expected index = -1 (not found)
        key_sig.write(10);
        wait(1, SC_NS);
        assert(index_sig.read() == -1);
        std::cout << "Test 2 Passed: 10 not found (index: " << index_sig.read() << ")" << std::endl;

        // Test Case 3: key = 1; expected index = 0
        key_sig.write(1);
        wait(1, SC_NS);
        assert(index_sig.read() == 0);
        std::cout << "Test 3 Passed: 1 found at index " << index_sig.read() << std::endl;

        std::cout << "All tests completed successfully." << std::endl;
        
        // End simulation after tests
        sc_stop();
    }

    // Destructor to clean up dynamically allocated memory
    ~Testbench() {
        delete ls_inst;
    }
};

// sc_main: entry point for the SystemC simulation
int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();  // Start the simulation
    return 0;
}
