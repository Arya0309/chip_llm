
#include <systemc.h>
#include <iostream>

// Module that rotates an integer array by a specified distance

SC_MODULE(Rotator) {
    sc_in<int> in_array[7]; // Input array
    sc_in<int> d; // Rotating distance
    sc_in<int> N; // Size of input array
    sc_out<int> out_array[7]; // Output array

    // Rotate process
    SC_THREAD(rotate) {
        // Temporary array to store rotated version
        int temp[7];

        // Keep track of current index
        int k = 0;

        // Rotate the first n - d elements of in_array to the front of temp
        for (int i = d; i < N; i++) {
            temp[k] = in_array[i];
            k++;
        }

        // Rotate the first d elements of in_array into temp
        for (int i = 0; i < d; i++) {
            temp[k] = in_array[i];
            k++;
        }

        // Copy the elements of temp back into out_array
        for (int i = 0; i < N; i++) {
            out_array[i] = temp[i];
        }

        // Copy the elements of out_array to in_array to ensure modular arithmetic
        for (int i = 0; i < N; i++) {
            in_array[i] = out_array[i];
        }
    }

    // Print process
    SC_THREAD(print) {
        // Print the elements of out_array to the console
        for (int i = 0; i < N; i++) {
            std::cout << out_array[i] << " ";
        }
    }

    // Constructor
    SC_CTOR(Rotator) {
        // Process to perform rotation
        SC_METHOD(rotate);
        sensitive << in_array << d << N;

        // Process to print the rotated array
        SC_METHOD(print);
        sensitive << out_array;
    }
};


SC_MODULE(Testbench) {
    sc_signal<bool> clk;
    sc_signal<bool> rst;
    sc_signal<int> d;
    sc_signal<int> in_array[7];
    sc_signal<int> out_array[7];

    Rotator* rotator_inst;

    // Clock generator process
    void generate_clock() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }

    // Process to run tests
    void run_tests() {
        // Activate reset for one clock cycle
        rst.write(true);
        wait(10, SC_NS);
        rst.write(false);

        // Provide the input array and rotation offset (d = 2)
        int input[7] = { 1, 2, 3, 4, 5, 6, 7 };
        for (int i = 0; i < 7; i++) {
            in_array[i].write(input[i]);
        }
        d.write(2);

        // Wait for the rotator to process the new values.
        wait(10, SC_NS);

        // Expected output after rotation: {3, 4, 5, 6, 7, 1, 2}
        int expected[7] = { 3, 4, 5, 6, 7, 1, 2 };
        std::cout << "Rotated Array: ";
        for (int i = 0; i < 7; i++) {
            int out_val = out_array[i].read();
            // Assert that the output is as expected.
            assert(out_val == expected[i]);
            std::cout << out_val << " ";
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // End simulation.
    }

    SC_CTOR(Testbench) {
        rotator_inst = new Rotator("Rotator");
        // Connect testbench signals to Rotator ports.
        rotator_inst->clk(clk);
        rotator_inst->rst(rst);
        rotator_inst->d(d);
        for (int i = 0; i < 7; i++) {
            rotator_inst->in_array[i](in_array[i]);
            rotator_inst->out_array[i](out_array[i]);
        }
        SC_THREAD(run_tests);
        SC_THREAD(generate_clock);
    }

    ~Testbench() {
        delete rotator_inst;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
