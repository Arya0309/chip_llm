
#include <systemc.h>

// Rotator Module: Implements the array rotation logic

SC_MODULE(Rotator) {
    sc_in<bool> clk;          // Clock input
    sc_in<bool> rst;          // Reset input
    sc_in<int> d;             // Number of positions to rotate
    sc_in<int> in_array[7];   // Input array
    sc_out<int> out_array[7]; // Output array

    // Internal storage for the array
    int arr[7];
    int temp[7];
    bool done;

    // Constructor: register the process
    SC_CTOR(Rotator) {
        SC_METHOD(rotate_process);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(output_process);
        sensitive << done;
        dont_initialize();
    }

    // Process that waits for the clock and performs rotation
    void rotate_process() {
        if (rst.read()) {
            done = false;
            for (int i = 0; i < 7; i++) {
                arr[i] = in_array[i].read();
            }
        } else if (!done) {
            int shift = d.read();
            int n = 7;

            // Store the n - d elements of arr[] to the front of temp[]
            for (int i = shift; i < n; i++) {
                temp[i - shift] = arr[i];
            }

            // Store the first d elements of arr[] into temp
            for (int i = 0; i < shift; i++) {
                temp[n - shift + i] = arr[i];
            }

            // Copy the elements of temp[] in arr[] to get the final rotated array
            for (int i = 0; i < n; i++) {
                arr[i] = temp[i];
            }

            done = true;
        }
    }

    // Process that updates the output array when rotation is done
    void output_process() {
        if (done) {
            for (int i = 0; i < 7; i++) {
                out_array[i].write(arr[i]);
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
