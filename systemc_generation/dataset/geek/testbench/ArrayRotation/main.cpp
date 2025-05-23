#include <systemc.h>
#include <cassert>
#include <iostream>

// Rotator module implementing the rotation functionality
SC_MODULE(Rotator) {
    // Ports for clock, reset, rotation offset, input array, and output array.
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> rst;          // Reset signal
    sc_in<int> d;             // Rotation offset
    sc_in<int> in_array[7];   // Input array (7 elements)
    sc_out<int> out_array[7]; // Output rotated array (7 elements)

    void do_rotate() {
        if (rst.read() == true) {
            // On reset, clear output array.
            for (int i = 0; i < 7; i++) {
                out_array[i].write(0);
            }
        } else {
            // Read input array into a local array.
            int arr[7];
            for (int i = 0; i < 7; i++) {
                arr[i] = in_array[i].read();
            }

            // Temporary array to store rotated elements.
            int temp[7];
            int k = 0;
            int rotation = d.read();

            // Copy elements from index 'rotation' to end.
            for (int i = rotation; i < 7; i++) {
                temp[k++] = arr[i];
            }
            // Copy first 'rotation' elements.
            for (int i = 0; i < rotation; i++) {
                temp[k++] = arr[i];
            }
            // Write the rotated array to output ports.
            for (int i = 0; i < 7; i++) {
                out_array[i].write(temp[i]);
            }
        }
    }

    SC_CTOR(Rotator) {
        SC_METHOD(do_rotate);
        sensitive << clk.pos();
    }
};

// Testbench module to test the Rotator module
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
