
#include <systemc.h>

// Linear Search Module
SC_MODULE(LinearSearchModule) {
    sc_in<sc_vector<sc_in<int>>> vec_in;  // Input vector of integers
    sc_in<int> key_in;                     // Input key to search
    sc_out<int> result_out;                // Output result (index or -1)

    int vec_size;

    // Constructor
    SC_CTOR(LinearSearchModule, const char* name, int size) : sc_module(name), vec_size(size) {
        // Process to perform linear search
        SC_METHOD(search);
        sensitive << key_in;
        for (int i = 0; i < vec_size; ++i) {
            sensitive << vec_in[i];
        }
    }

    // Method to perform linear search
    void search() {
        int key = key_in.read();
        for (int i = 0; i < vec_size; ++i) {
            if (vec_in[i].read() == key) {
                result_out.write(i);
                return;
            }
        }
        result_out.write(-1);
    }
};

// Testbench Module
SC_MODULE(Testbench) {
    sc_vector<sc_signal<int>> vec_sig;    // Signals for the vector elements
    sc_signal<int> key_sig;               // Signal for the key
    sc_signal<int> result_sig;            // Signal for the result

    LinearSearchModule ls_inst;           // Instance of the LinearSearchModule

    // Constructor
    SC_CTOR(Testbench) : ls_inst("ls_inst", 8) {
        // Connect signals to module ports
        ls_inst.vec_in(vec_sig);
        ls_inst.key_in(key_sig);
        ls_inst.result_out(result_sig);

        // Initialize vector with given values
        vec_sig[0].write(1);
        vec_sig[1].write(2);
        vec_sig[2].write(3);
        vec_sig[3].write(4);
        vec_sig[4].write(5);
        vec_sig[5].write(8);
        vec_sig[6].write(9);
        vec_sig[7].write(11);

        // Set key to search
        key_sig.write(8);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Wait for the linear search to complete
        wait(1, SC_NS);

        // Check the result
        int result = result_sig.read();
        if (result != -1) {
            cout << "Key " << key_sig.read() << " Found at Position: " << result + 1 << endl;
        } else {
            cout << "Key " << key_sig.read() << " NOT found." << endl;
        }

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
