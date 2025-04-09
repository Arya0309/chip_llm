
#include <systemc.h>

// Module that performs linear search

SC_MODULE(LinearSearch) {
    sc_in<int> key;  // Input port for the key to search
    sc_out<int> index; // Output port for the index of the key
    sc_vector< sc_in<int> > data; // Vector of input ports for the data elements

    // Constructor
    SC_CTOR(LinearSearch) : data("data", 8) { // Assuming fixed size of 8 for simplicity
        // Process to perform linear search
        SC_METHOD(search);
        sensitive << key;
        for (int i = 0; i < data.size(); ++i) {
            sensitive << data[i];
        }
    }

    // Method to perform the linear search
    void search() {
        int foundIndex = -1;
        for (int i = 0; i < data.size(); ++i) {
            if (data[i].read() == key.read()) {
                foundIndex = i;
                break;
            }
        }
        index.write(foundIndex);
    }
};

// Example top module to demonstrate usage

SC_MODULE(Top) {
    sc_signal<int> keySignal;
    sc_signal<int> indexSignal;
    sc_signal<int> dataSignals[8];

    LinearSearch ls;

    SC_CTOR(Top) : ls("ls") {
        ls.key(keySignal);
        ls.index(indexSignal);
        for (int i = 0; i < 8; ++i) {
            ls.data[i](dataSignals[i]);
        }

        // Initialize data
        SC_THREAD(initialize_data);
    }

    void initialize_data() {
        int data_values[] = {1, 2, 3, 4, 5, 8, 9, 11};
        for (int i = 0; i < 8; ++i) {
            dataSignals[i].write(data_values[i]);
        }
        wait(1, SC_NS); // Wait for the data to stabilize
        keySignal.write(8); // Set the key to search
        wait(1, SC_NS); // Wait for the search to complete
        cout << "Key " << keySignal.read() << " ";
        if (indexSignal.read() != -1) {
            cout << "Found at Position: " << indexSignal.read() + 1 << endl;
        } else {
            cout << "NOT found." << endl;
        }
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
