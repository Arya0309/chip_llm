
#include <systemc.h>

// Module that performs linear search
SC_MODULE(LinearSearchModule) {
    sc_in<sc_uint<32>> key;            // Input port for the key to search
    sc_inout<sc_uint<32>> search_val;   // Input/output port for the current value being searched
    sc_inout<bool> search_done;        // Output port to indicate if search is complete
    sc_inout<sc_int<32>> result;        // Output port to store the result of the search (-1 if not found)

    // Constructor
    SC_CTOR(LinearSearchModule) {
        // Process to perform linear search
        SC_METHOD(search);
        sensitive << search_val << key;
    }

    // Method to perform linear search
    void search() {
        static int i = 0;
        static bool searching = false;

        if (!searching) {
            i = 0;
            searching = true;
            search_val.write(i);
        } else {
            if (search_val.read() == key.read()) {
                result.write(i);
                searching = false;
                search_done.write(true);
            } else if (i < 7) { // Assuming vector size is 8, so max index is 7
                i++;
                search_val.write(i);
            } else {
                result.write(-1);
                searching = false;
                search_done.write(true);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_uint<32>> key;         // Signal for the key to search
    sc_signal<sc_uint<32>> search_val;  // Signal for the current value being searched
    sc_signal<bool> search_done;        // Signal to indicate if search is complete
    sc_signal<sc_int<32>> result;       // Signal to store the result of the search

    LinearSearchModule search_module;   // Instance of the LinearSearchModule

    // Constructor
    SC_CTOR(Testbench) : search_module("search_module") {
        // Connect signals to ports
        search_module.key(key);
        search_module.search_val(search_val);
        search_module.search_done(search_done);
        search_module.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize vector values
        sc_uint<32> vector_values[8] = {1, 2, 3, 4, 5, 8, 9, 11};

        // Set the key to search
        key = 8;
        search_done.write(false);
        wait(1, SC_NS); // Wait for the search to start

        // Simulate the vector search
        while (!search_done.read()) {
            search_val.write(vector_values[search_val.read()]);
            wait(1, SC_NS);
        }

        // Output the result
        if (result.read() != -1)
            cout << "Key " << key.read() << " Found at Position: " << result.read() + 1 << endl;
        else
            cout << "Key " << key.read() << " NOT found." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
