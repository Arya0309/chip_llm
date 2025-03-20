
#include <systemc.h>

// Module to perform array rotation
SC_MODULE(ArrayRotator) {
    // Input port for the number of rotations
    sc_in<int> num_rotations;
    
    // Input port for the array size
    sc_in<int> array_size;
    
    // Input and output ports for the array
    sc_inout<int> array[7]; // Assuming a fixed size of 7 for simplicity
    
    // Method to rotate the array
    void rotate_array() {
        int d = num_rotations.read();
        int n = array_size.read();
        int temp[7]; // Temporary array for rotation
        
        // Copy elements from index d to end of array to temp
        int k = 0;
        for (int i = d; i < n; i++) {
            temp[k++] = array[i].read();
        }
        
        // Copy first d elements of array to end of temp
        for (int i = 0; i < d; i++) {
            temp[k++] = array[i].read();
        }
        
        // Copy temp back to array
        for (int i = 0; i < n; i++) {
            array[i].write(temp[i]);
        }
    }
    
    // Constructor to register the process
    SC_CTOR(ArrayRotator) {
        SC_METHOD(rotate_array);
        sensitive << num_rotations << array_size;
        for (int i = 0; i < 7; i++) {
            sensitive << array[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with ArrayRotator
    sc_signal<int> num_rotations;
    sc_signal<int> array_size;
    sc_signal<int> array[7];
    
    // Instance of ArrayRotator
    ArrayRotator rotator;
    
    // Process to drive inputs and print outputs
    void drive_and_print() {
        // Initialize array
        for (int i = 0; i < 7; i++) {
            array[i].write(i + 1);
        }
        // Set number of rotations and array size
        num_rotations = 2;
        array_size = 7;
        
        // Wait for one delta cycle to let the rotator process
        wait(1, SC_NS);
        
        // Print the results
        cout << "Rotated array: ";
        for (int i = 0; i < 7; i++) {
            cout << array[i].read() << " ";
        }
        cout << endl;
    }
    
    // Constructor to register the process
    SC_CTOR(Testbench) : rotator("rotator") {
        // Connecting signals to the rotator ports
        rotator.num_rotations(num_rotations);
        rotator.array_size(array_size);
        for (int i = 0; i < 7; i++) {
            rotator.array[i](array[i]);
        }
        
        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
