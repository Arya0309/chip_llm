
#include <systemc.h>

// Module to reverse an array
SC_MODULE(ArrayReverser) {
    // Input and output ports for arrays
    sc_vector<sc_in<int>> original_arr;
    sc_vector<sc_out<int>> copied_arr;
    
    // Length of the array
    int len;

    // Process to reverse the array
    void reverseArray() {
        // Copy the elements of the array in the copied_arr in Reverse Order
        for (int i = 0; i < len; i++) {
            copied_arr[i].write(original_arr[len - i - 1].read());
        }
    }

    // Constructor to register the process
    SC_CTOR(ArrayReverser, const char* name, int array_length) : sc_module(name), len(array_length) {
        original_arr.init(len);
        copied_arr.init(len);
        
        SC_METHOD(reverseArray);
        sensitive << original_arr;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Length of the array
    int len = 5;

    // Signals to connect with ArrayReverser
    sc_vector<sc_signal<int>> original_arr;
    sc_vector<sc_signal<int>> copied_arr;

    // Instance of ArrayReverser
    ArrayReverser reverser;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the original array
        for (int i = 0; i < len; i++) {
            original_arr[i].write(i + 1); // Writing values {1, 2, 3, 4, 5}
        }

        // Wait for one delta cycle to let the reverser process
        wait(1, SC_NS);

        // Print the original array
        cout << "Original array: ";
        for (int i = 0; i < len; i++) {
            cout << original_arr[i].read() << " ";
        }
        cout << endl;

        // Print the resultant array
        cout << "Resultant array: ";
        for (int i = 0; i < len; i++) {
            cout << copied_arr[i].read() << " ";
        }
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : reverser("reverser", len) {
        // Connecting signals to the reverser ports
        for (int i = 0; i < len; i++) {
            reverser.original_arr[i](original_arr[i]);
            reverser.copied_arr[i](copied_arr[i]);
        }

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
