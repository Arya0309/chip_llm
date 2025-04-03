
#include <systemc.h>

// Module to remove all occurrences of a target element from an array
SC_MODULE(RemoveOccurrences) {
    sc_in<int> arr[11]; // Input array (fixed size for demonstration)
    sc_in<int> target;  // Target element to be removed
    sc_in<int> n;       // Size of the input array
    sc_out<int> new_arr[8]; // Output array (max size after removal)

    // Constructor
    SC_CTOR(RemoveOccurrences) {
        // Process to remove occurrences
        SC_METHOD(remove_occurrences);
        sensitive << target << n;
        for(int i = 0; i < 11; ++i)
            sensitive << arr[i];
    }

    // Method to remove occurrences
    void remove_occurrences() {
        int cnt = 0;

        // Count occurrences of target
        for(int i = 0; i < n.read(); i++) {
            if(arr[i].read() == target.read())
                cnt++;
        }

        // Create new array
        int ind = 0;
        for(int i = 0; i < n.read(); i++) {
            if(arr[i].read() != target.read()) {
                new_arr[ind].write(arr[i].read());
                ind++;
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> arr[11]; // Signals for the input array
    sc_signal<int> target;  // Signal for the target element
    sc_signal<int> n;       // Signal for the size of the array
    sc_signal<int> new_arr[8]; // Signals for the output array

    RemoveOccurrences remove_occurrences_inst; // Instance of the RemoveOccurrences module

    // Constructor
    SC_CTOR(Testbench) : remove_occurrences_inst("remove_occurrences") {
        // Connect signals to ports
        for(int i = 0; i < 11; ++i)
            remove_occurrences_inst.arr[i](arr[i]);
        remove_occurrences_inst.target(target);
        remove_occurrences_inst.n(n);
        for(int i = 0; i < 8; ++i)
            remove_occurrences_inst.new_arr[i](new_arr[i]);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize input array and target
        for(int i = 0; i < 11; ++i)
            arr[i] = i == 2 || i == 5 || i == 8 || i == 9 ? 3 : i + 1;
        target = 3;
        n = 11;

        // Wait for the module to process
        wait(1, SC_NS);

        // Print the new array
        cout << "New array: ";
        for(int i = 0; i < 8; ++i) {
            cout << new_arr[i].read() << " ";
        }
        cout << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
