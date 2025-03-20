
#include <systemc.h>

// Define a module to remove all occurrences of a target value from an array
SC_MODULE(RemoveOccurrences) {
    // Input ports
    sc_in<sc_uint<32>> target;  // Target value to remove
    sc_in<sc_uint<32>> n;       // Number of elements in the array
    sc_vector<sc_in<sc_uint<32>>> arr;  // Input array

    // Output port for the modified array size
    sc_out<sc_uint<32>> modified_size;

    // Output ports for the modified array elements
    sc_vector<sc_out<sc_uint<32>>> modified_arr;

    // Internal storage for the modified array
    sc_uint<32> new_arr[100];  // Assuming a maximum array size of 100 for simplicity

    // Process to remove occurrences
    void remove_occurrences() {
        // Read inputs
        sc_uint<32> target_val = target.read();
        sc_uint<32> num_elements = n.read();

        // Count occurrences of target
        sc_uint<32> cnt = 0;
        for (sc_uint<32> i = 0; i < num_elements; i++) {
            if (arr[i].read() == target_val) {
                cnt++;
            }
        }

        // Create new array without target occurrences
        sc_uint<32> ind = 0;
        for (sc_uint<32> i = 0; i < num_elements; i++) {
            if (arr[i].read() != target_val) {
                new_arr[ind] = arr[i].read();
                ind++;
            }
        }

        // Write the modified array size
        modified_size.write(ind);

        // Write the modified array elements
        for (sc_uint<32> i = 0; i < ind; i++) {
            modified_arr[i].write(new_arr[i]);
        }
    }

    // Constructor to register the process
    SC_CTOR(RemoveOccurrences) : arr("arr", 100), modified_arr("modified_arr", 100) {
        SC_METHOD(remove_occurrences);
        sensitive << target << n;
        for (sc_uint<32> i = 0; i < 100; i++) {
            sensitive << arr[i];
        }
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with RemoveOccurrences
    sc_signal<sc_uint<32>> target;
    sc_signal<sc_uint<32>> n;
    sc_vector<sc_signal<sc_uint<32>>> arr;
    sc_signal<sc_uint<32>> modified_size;
    sc_vector<sc_signal<sc_uint<32>>> modified_arr;

    // Instance of RemoveOccurrences
    RemoveOccurrences remover;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize array and target
        for (sc_uint<32> i = 0; i < 11; i++) {
            arr[i].write(i < 11 ? sc_uint<32>({1, 4, 3, 6, 8, 3, 9, 10, 3, 3, 7}[i]) : 0);
        }
        target.write(3);
        n.write(11);

        // Wait for one delta cycle to let the remover process
        wait(1, SC_NS);

        // Print the results
        std::cout << "Modified array: ";
        for (sc_uint<32> i = 0; i < modified_size.read(); i++) {
            std::cout << modified_arr[i].read() << " ";
        }
        std::cout << std::endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : arr("arr", 100), modified_arr("modified_arr", 100), remover("remover") {
        // Connecting signals to the remover ports
        remover.target(target);
        remover.n(n);
        for (sc_uint<32> i = 0; i < 100; i++) {
            remover.arr[i](arr[i]);
            remover.modified_arr[i](modified_arr[i]);
        }
        remover.modified_size(modified_size);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
