scss
#include <systemc.h>


SC_MODULE(LinearSearch) {
    sc_in< vector<int> > v;  // Input port for the vector v
    sc_in< int > key;  // Input port for the key value
    sc_out< int > index;  // Output port for the index of the key
    sc_out< bool > found;  // Output port for whether the key was found or not

    SC_METHOD(search);
    sensitive << v << key;
    
    void search() {
        // Local variables to hold the size of the vector and the index of the key
        int size = v.size();
        int i = -1;
        
        // Loop over the elements of the vector
        for (int i = 0; i < size; i++) {
            // If the key value is found in the vector, store its index in the output port
            if (v[i] == key) {
                i = i;
                index.write(i);
                found.write(true);
            }
        }
        
        // If the key value was not found in the vector, store -1 in the output port
        if (i == -1) {
            index.write(-1);
            found.write(false);
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
