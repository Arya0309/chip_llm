
#include <systemc.h>

// Module that rotates an array

SC_MODULE(Rotator) {
    // Inputs
    sc_in<int> a[7];     // Original input array
    sc_in<int> d;        // Shift amount

    // Outputs
    sc_out<int> temp[7];  // Temporary array
    sc_out<int> out_array[7];  // Rotated output array

    // Constructor
    SC_CTOR(Rotator) {
        // Process to rotate array
        SC_METHOD(rotate);
        sensitive << a << d;

        // Process to print rotated array
        SC_METHOD(print_array);
        sensitive << temp;
    }

    // Method to rotate array
    void rotate() {
        // Creating a temporary array
        temp.write(a);

        // Rotating the array by d elements
        for (int i = 0; i < d; i++) {
            temp[i] = a[i+d];
            temp[i+d] = a[i];
        }
    }

    // Method to print rotated array
    void print_array() {
        cout << "Rotated array: ";
        for (int i = 0; i < 7; i++) {
            cout << temp[i] << " ";
        }
        cout << endl;
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
