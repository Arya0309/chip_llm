
#include <systemc.h>


SC_MODULE(Rotator) {
    // Inputs
    sc_in<bool> clk;
    sc_in<bool> rst;
    sc_in<int> d;
    sc_in<int> in_array[7];

    // Output
    sc_out<int> out_array[7];

    // Internal signals
    sc_signal<int> arr[7];
    sc_signal<int> temp[7];
    sc_signal<int> n;
    sc_signal<bool> rotating;
    sc_signal<int> k;

    void rotate() {
        if (rst.read()) {
            // Reset all signals
            for (int i = 0; i < 7; ++i) {
                arr[i].write(0);
                temp[i].write(0);
                out_array[i].write(0);
            }
            n.write(0);
            rotating.write(false);
            k.write(0);
        } else if (clk.event() && rotating.read()) {
            if (k.read() < n.read()) {
                if (k.read() < n.read() - d.read()) {
                    temp[k.read()].write(arr[d.read() + k.read()].read());
                } else {
                    temp[k.read()].write(arr[k.read() - (n.read() - d.read())].read());
                }
                k.write(k.read() + 1);
            } else {
                // Copy temp to out_array
                for (int i = 0; i < 7; ++i) {
                    out_array[i].write(temp[i].read());
                }
                rotating.write(false);
            }
        }
    }

    void start_rotation() {
        if (rst.read()) {
            rotating.write(false);
        } else if (clk.event() && !rotating.read()) {
            // Initialize internal signals
            for (int i = 0; i < 7; ++i) {
                arr[i].write(in_array[i].read());
            }
            n.write(7); // Fixed size of 7
            k.write(0);
            rotating.write(true);
        }
    }

    SC_CTOR(Rotator) {
        SC_METHOD(start_rotation);
        sensitive << clk.pos();

        SC_METHOD(rotate);
        sensitive << clk.pos();
        sensitive << rst.pos();
    }
};

// Example testbench


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
