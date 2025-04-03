
#include <systemc.h>

// Module to rotate an array
SC_MODULE(ArrayRotator) {
    sc_in<int> d; // Number of positions to rotate
    sc_in<int> N; // Size of the array
    sc_vector<sc_in<int>> arr_in; // Input array
    sc_vector<sc_out<int>> arr_out; // Output array

    // Constructor
    SC_CTOR(ArrayRotator, const char* name, int size) : sc_module(name), arr_in("arr_in", size), arr_out("arr_out", size) {
        // Process to perform rotation
        SC_METHOD(rotate);
        sensitive << d << N;
        for (int i = 0; i < size; i++) {
            sensitive << arr_in[i];
        }
    }

    // Method to rotate the array
    void rotate() {
        int d_val = d.read();
        int N_val = N.read();
        int temp[N_val];

        // Keeping track of the current index of temp[]
        int k = 0;

        // Storing the n - d elements of array arr[] to the front of temp[]
        for (int i = d_val; i < N_val; i++) {
            temp[k] = arr_in[i].read();
            k++;
        }

        // Storing the first d elements of array arr[] into temp
        for (int i = 0; i < d_val; i++) {
            temp[k] = arr_in[i].read();
            k++;
        }

        // Copying the elements of temp[] in arr[] to get the final rotated array
        for (int i = 0; i < N_val; i++) {
            arr_out[i].write(temp[i]);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> d; // Number of positions to rotate
    sc_signal<int> N; // Size of the array
    sc_vector<sc_signal<int>> arr_in; // Input array
    sc_vector<sc_signal<int>> arr_out; // Output array

    ArrayRotator rotator_inst; // Instance of the ArrayRotator module

    // Constructor
    SC_CTOR(Testbench) : rotator_inst("rotator", arr_in.size()) {
        // Connect signals to ports
        rotator_inst.d(d);
        rotator_inst.N(N);
        for (size_t i = 0; i < arr_in.size(); i++) {
            rotator_inst.arr_in[i](arr_in[i]);
            rotator_inst.arr_out[i](arr_out[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize array
        int initial_arr[] = {1, 2, 3, 4, 5, 6, 7};
        int N_val = sizeof(initial_arr) / sizeof(initial_arr[0]);
        int d_val = 2;

        // Set the size and rotation amount
        N.write(N_val);
        d.write(d_val);

        // Set the initial array values
        for (int i = 0; i < N_val; i++) {
            arr_in[i].write(initial_arr[i]);
        }

        // Wait for the rotator to process
        wait(1, SC_NS);

        // Print the rotated array
        for (int i = 0; i < N_val; i++) {
            cout << arr_out[i].read() << " ";
        }
        cout << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    // Initialize array
    int initial_arr[] = {1, 2, 3, 4, 5, 6, 7};
    int N_val = sizeof(initial_arr) / sizeof(initial_arr[0]);

    // Create an instance of the Testbench
    Testbench tb("tb", N_val);

    // Start the simulation
    sc_start();

    return 0;
}
