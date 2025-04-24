
#include <systemc.h>


SC_MODULE(Rotator) {
    // Module that rotates an array of integers
    sc_in<sc_uint<32> > arr[7];  // Input array of integers
    sc_in<sc_uint<32> > d;  // Rotation distance
    sc_out<sc_uint<32> > out_array[7];  // Output array of integers

    // Constructor
    SC_CTOR(Rotator) {
        // Rotate the array
        SC_METHOD(Rotate);
        sensitive << arr << d;

        // Print the array
        SC_METHOD(PrintTheArray);
        sensitive << arr << N;
    }

    // Method to rotate the array
    void Rotate() {
        sc_uint<32> temp[7];  // Temporary array
        sc_uint<32> k = 0;  // Current index in temp array

        // Copy the latter half of arr to the beginning of temp
        for (int i = d.read(); i < 7; i++) {
            temp[k] = arr[i].read();
            k++;
        }

        // Copy the first half of arr to the end of temp
        for (int i = 0; i < d.read(); i++) {
            temp[k] = arr[i].read();
            k++;
        }

        // Copy back to output array
        for (int i = 0; i < 7; i++) {
            out_array[i] = temp[i];
        }
    }

    // Method to print the array
    void PrintTheArray() {
        for (int i = 0; i < 7; i++) {
            cout << out_array[i] << " ";
        }
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
