
#include <systemc.h>

// User Defined Linear Search Module
SC_MODULE(LinearSearchModule) {
    // Input port for the key to search
    sc_in<int> key_in;

    // Output port for the result (position or -1 if not found)
    sc_out<int> result_out;

    // Internal vector to hold the data
    std::vector<int> v;

    // Process to perform linear search
    void performSearch() {
        int key = key_in.read();
        int result = -1;

        // Perform linear search
        for (int i = 0; i < v.size(); i++) {
            if (v[i] == key) {
                result = i + 1; // Convert to 1-based index
                break;
            }
        }

        // Write the result to the output port
        result_out.write(result);
    }

    // Constructor to initialize the vector and register the process
    SC_CTOR(LinearSearchModule) : v({1, 2, 3, 4, 5, 8, 9, 11}) {
        SC_METHOD(performSearch);
        sensitive << key_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with LinearSearchModule
    sc_signal<int> key_sig;
    sc_signal<int> result_sig;

    // Instance of LinearSearchModule
    LinearSearchModule searcher;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the key to search
        key_sig = 8;

        // Wait for one delta cycle to let the search process
        wait(1, SC_NS);

        // Print the result
        int result = result_sig.read();
        if (result != -1)
            cout << "Key " << key_sig.read() << " Found at Position: " << result << endl;
        else
            cout << "Key " << key_sig.read() << " NOT found." << endl;
    }

    // Constructor to register the process and connect signals
    SC_CTOR(Testbench) : searcher("searcher") {
        // Connecting signals to the searcher ports
        searcher.key_in(key_sig);
        searcher.result_out(result_sig);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
