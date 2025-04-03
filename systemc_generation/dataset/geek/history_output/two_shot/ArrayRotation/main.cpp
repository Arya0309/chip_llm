
#include <systemc.h>

// Module to rotate an array
SC_MODULE(ArrayRotator) {
    sc_in<int> d;          // Input for the number of positions to rotate
    sc_in<int> n;          // Input for the size of the array
    sc_vector<sc_in<int>> arr_in; // Input vector for the array elements
    sc_vector<sc_out<int>> arr_out; // Output vector for the rotated array elements

    // Constructor
    SC_CTOR(ArrayRotator) : arr_in("arr_in", n), arr_out("arr_out", n) {
        // Process to perform rotation
        SC_METHOD(rotate_array);
        sensitive << d << n;
        for (int i = 0; i < n; i++) {
            sensitive << arr_in[i];
        }
    }

    // Method to rotate the array
    void rotate_array() {
        int temp[n];
        int k = 0;

        // Copy elements from index d to n-1
        for (int i = d.read(); i < n.read(); i++) {
            temp[k] = arr_in[i].read();
            k++;
        }

        // Copy elements from index 0 to d-1
        for (int i = 0; i < d.read(); i++) {
            temp[k] = arr_in[i].read();
            k++;
        }

        // Copy rotated elements back to output array
        for (int i = 0; i < n.read(); i++) {
            arr_out[i].write(temp[i]);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> d;      // Signal for the number of positions to rotate
    sc_signal<int> n;      // Signal for the size of the array
    sc_vector<sc_signal<int>> arr_in; // Signals for the array elements
    sc_vector<sc_signal<int>> arr_out; // Signals for the rotated array elements

    ArrayRotator rotator_inst; // Instance of the ArrayRotator module

    // Constructor
    SC_CTOR(Testbench) : arr_in("arr_in", n), arr_out("arr_out", n), rotator_inst("rotator") {
        // Connect signals to ports
        rotator_inst.d(d);
        rotator_inst.n(n);
        for (int i = 0; i < n.read(); i++) {
            rotator_inst.arr_in[i](arr_in[i]);
            rotator_inst.arr_out[i](arr_out[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array size
        n = 7;

        // Initialize array elements
        int initial_arr[] = { 1, 2, 3, 4, 5, 6, 7 };

        for (int i = 0; i < n.read(); i++) {
            arr_in[i] = initial_arr[i];
        }

        // Set rotation amount
        d = 2;

        // Wait for the rotator to process
        wait(1, SC_NS);

        // Print rotated array
        cout << "Rotated Array: ";
        for (int i = 0; i < n.read(); i++) {
            cout << arr_out[i].read() << " ";
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
