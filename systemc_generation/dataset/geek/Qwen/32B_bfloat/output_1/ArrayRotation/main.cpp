
#include <systemc.h>


SC_MODULE(Rotator) {
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<int> d;
    sc_in<int> in_array[7];
    sc_out<int> out_array[7];

    SC_CTOR(Rotator) {
        SC_METHOD(rotate_and_output);
        sensitive << clk.pos();
        dont_initialize();
    }

    void rotate_and_output() {
        if (rst.read() == true) {
            // Reset all outputs to 0
            for (int i = 0; i < 7; ++i) {
                out_array[i].write(0);
            }
        } else {
            int arr[7];
            int temp[7];
            int shift = d.read();

            // Read input array
            for (int i = 0; i < 7; ++i) {
                arr[i] = in_array[i].read();
            }

            // Rotate the array
            int k = 0;
            for (int i = shift; i < 7; i++) {
                temp[k] = arr[i];
                k++;
            }
            for (int i = 0; i < shift; i++) {
                temp[k] = arr[i];
                k++;
            }

            // Write rotated array to output
            for (int i = 0; i < 7; i++) {
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
