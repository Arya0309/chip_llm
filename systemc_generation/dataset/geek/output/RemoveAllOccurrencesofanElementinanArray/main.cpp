
#include <systemc.h>
#include <vector>
#include <iostream>

// Module to remove all occurrences of a target element from an array
SC_MODULE(RemoveOccurrences) {
    sc_in<sc_uint<32>> target; // Target element to remove
    sc_in<sc_uint<32>> arr_size; // Size of the input array
    sc_vector<sc_in<sc_uint<32>>> arr; // Input array
    sc_out<sc_vector<sc_uint<32>>> result; // Resulting array without target elements

    // Constructor
    SC_CTOR(RemoveOccurrences) : arr("arr", arr_size), result("result", arr_size) {
        // Process to remove occurrences
        SC_METHOD(remove_occurrences);
        sensitive << arr_size;
    }

    // Method to remove occurrences of target from array
    void remove_occurrences() {
        std::vector<sc_uint<32>> new_arr;
        int cnt = 0;

        // Count occurrences of target
        for (int i = 0; i < arr_size.read(); i++) {
            if (arr[i].read() == target.read()) {
                cnt++;
            }
        }

        // Create new array without target elements
        new_arr.resize(arr_size.read() - cnt);
        int ind = 0;
        for (int i = 0; i < arr_size.read(); i++) {
            if (arr[i].read() != target.read()) {
                new_arr[ind++] = arr[i].read();
            }
        }

        // Write result to output vector
        for (int i = 0; i < new_arr.size(); i++) {
            result[i].write(new_arr[i]);
        }

        // Print the new array
        for (int i = 0; i < new_arr.size(); i++) {
            std::cout << new_arr[i] << " ";
        }
        std::cout << std::endl;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<sc_uint<32>> target; // Target element to remove
    sc_signal<sc_uint<32>> arr_size; // Size of the input array
    sc_vector<sc_signal<sc_uint<32>>> arr; // Input array
    sc_vector<sc_signal<sc_uint<32>>> result; // Resulting array without target elements

    RemoveOccurrences remove_occurrences_inst; // Instance of the RemoveOccurrences module

    // Constructor
    SC_CTOR(Testbench) : arr("arr", arr_size), result("result", arr_size),
                         remove_occurrences_inst("remove_occurrences_inst") {
        // Connect signals to ports
        remove_occurrences_inst.target(target);
        remove_occurrences_inst.arr_size(arr_size);
        for (int i = 0; i < arr_size.read(); i++) {
            remove_occurrences_inst.arr[i](arr[i]);
            remove_occurrences_inst.result[i](result[i]);
        }

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize the array and target
        arr_size.write(11);
        arr[0].write(1);
        arr[1].write(4);
        arr[2].write(3);
        arr[3].write(6);
        arr[4].write(8);
        arr[5].write(3);
        arr[6].write(9);
        arr[7].write(10);
        arr[8].write(3);
        arr[9].write(3);
        arr[10].write(7);
        target.write(3);

        // Wait for the process to complete
        wait(1, SC_NS);

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
