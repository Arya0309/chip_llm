
#include <systemc.h>

// LinearSearch Module: Implements the linear search algorithm

SC_MODULE(LinearSearch) {
    sc_in<int> key;         // Input signal for the key to search
    sc_out<int> index;       // Output signal for the index of the key
    sc_vector< sc_in<int> > data; // Vector of input signals representing the data array

    // Constructor: register the process
    SC_CTOR(LinearSearch) {
        SC_METHOD(search_process);
        sensitive << key;
    }

    // Process that performs the linear search
    void search_process() {
        int search_key = key.read();
        int found_index = -1;

        // Iterate through the data vector to find the key
        for (int i = 0; i < data.size(); i++) {
            if (data[i].read() == search_key) {
                found_index = i;
                break;
            }
        }

        // Write the result to the index output
        index.write(found_index);
    }
};

// Example usage in a top-level module

SC_MODULE(TopLevel) {
    sc_signal<int> key_sig;       // Signal for the key
    sc_signal<int> index_sig;     // Signal for the index
    sc_vector< sc_signal<int> > data_sig; // Vector of signals for the data array

    LinearSearch ls_inst; // Instance of the LinearSearch module

    SC_CTOR(TopLevel) : data_sig("data_sig", 8), ls_inst("ls_inst") {
        ls_inst.key(key_sig);
        ls_inst.index(index_sig);

        // Connect the data vector signals
        for (int i = 0; i < data_sig.size(); i++) {
            ls_inst.data[i](data_sig[i]);
        }

        // Initialize the data vector with predefined values
        SC_METHOD(init_data);
        sensitive << sc_start_of_simulation_event();
    }

    // Method to initialize the data vector
    void init_data() {
        int data_values[] = {1, 2, 3, 4, 5, 8, 9, 11};
        for (int i = 0; i < data_sig.size(); i++) {
            data_sig[i].write(data_values[i]);
        }

        // Set the key to search
        key_sig.write(8);
    }
};



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
