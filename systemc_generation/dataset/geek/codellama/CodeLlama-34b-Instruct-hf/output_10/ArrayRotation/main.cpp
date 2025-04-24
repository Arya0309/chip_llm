scss
#include <systemc.h>

// Module that rotates an array of integers

SC_MODULE(Rotator) {
    sc_in<int> arr[7];  // Input port for the array to be rotated
    sc_in<int> d;  // Input port for the number of positions to rotate by
    sc_in<bool> rst;  // Input port for reset
    sc_out<int> out_array[7];  // Output port for the rotated array

    // Constructor
    SC_CTOR(Rotator) {
        // Process to perform rotation
        SC_METHOD(rotate);
        sensitive << arr << d << rst;
    }

    // Method to rotate the array
    void rotate() {
        int temp[7];
        for (int i = 0; i < 7; i++) {
            temp[i] = 0;
        }
        for (int i = d; i < 7; i++) {
            temp[i - d] = arr[i];
        }
        for (int i = 0; i < d; i++) {
            temp[7 - d + i] = arr[i];
        }
        for (int i = 0; i < 7; i++) {
            out_array[i] = temp[i];
        }
    }
};

// Module that prints an array of integers

SC_MODULE(Printer) {
    sc_in<int> arr[7];  // Input port for the array to be printed
    sc_in<bool> rst;  // Input port for reset
    sc_out<bool> out_array[7];  // Output port for the rotated array

    // Constructor
    SC_CTOR(Printer) {
        // Process to print the array
        SC_METHOD(print);
        sensitive << arr << rst;
    }

    // Method to print the array
    void print() {
        for (int i = 0; i < 7; i++) {
            sc_uint<32> val = arr[i].read();
            for (int j = 0; j < 32; j++) {
                if (val >> j & 1) {
                    out_array[i] = 1;
                } else {
                    out_array[i] = 0;
                }
            }
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
