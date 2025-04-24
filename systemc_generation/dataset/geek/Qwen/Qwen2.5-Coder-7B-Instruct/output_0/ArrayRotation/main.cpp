
#include <systemc.h>


SC_MODULE(Rotator) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> rst;  // Reset signal
    sc_in<int> d;     // Number of positions to rotate
    sc_in<int> in_array[7];  // Input array
    sc_out<int> out_array[7];  // Output array

    int arr[7];  // Internal array to store the input values
    int n = 7;   // Size of the array

    // Constructor
    SC_CTOR(Rotator) {
        // Process to handle rotation
        SC_METHOD(handle_rotation);
        sensitive << clk.pos();  // Sensitive to positive edge of clock
        dont_initialize();  // Do not initialize this method
    }

    // Method to handle rotation logic
    void handle_rotation() {
        if (rst.read()) {
            // Reset the output array
            for (int i = 0; i < n; i++) {
                out_array[i].write(0);
            }
        } else {
            // Copy input array to internal array
            for (int i = 0; i < n; i++) {
                arr[i] = in_array[i].read();
            }

            // Temporary array to store rotated values
            int temp[n];

            // Copy n - d elements to the front of temp
            for (int i = d; i < n; i++) {
                temp[i - d] = arr[i];
            }

            // Append the first d elements to temp
            for (int i = 0; i < d; i++) {
                temp[n - d + i] = arr[i];
            }

            // Copy temp back to output array
            for (int i = 0; i < n; i++) {
                out_array[i].write(temp[i]);
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
