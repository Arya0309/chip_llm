#include <systemc.h>

// LinearSearch module: performs a linear search over an externally provided data array.
SC_MODULE(LinearSearch) {
    sc_in<int> key;                     // Input port for the search key
    sc_vector< sc_in<int> > data;         // Vector of input ports for the data array
    sc_out<int> index;                  // Output port for the found index

    // Constructor accepts the number of elements in the data array.
    LinearSearch(sc_module_name name, int n) 
        : sc_module(name), data("data", n)
    {
        SC_METHOD(search);
        sensitive << key;  // Trigger when the search key changes
        // Optionally, you can also sensitize to data changes if needed
        for (int i = 0; i < n; i++) {
            sensitive << data[i];
        }
    }

    // Method to perform linear search over the data array.
    void search() {
        int found_index = -1;  // Default result: not found
        for (unsigned i = 0; i < data.size(); i++) {
            if (data[i].read() == key.read()) {
                found_index = i;  // Key found at index i
                break;
            }
        }
        index.write(found_index);
    }
};

// Testbench module: supplies test data to the LinearSearch module and verifies the output.
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
