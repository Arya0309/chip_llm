
#include <systemc.h>
#include <vector>

// Module to remove duplicates from an array
SC_MODULE(RemoveDuplicates) {
    sc_in<bool> clk;              // Clock signal
    sc_in<bool> rst;              // Reset signal
    sc_in<int> n;                 // Size of the array
    sc_in<int> arr_in[9];         // Input array (fixed size for simplicity)
    sc_out<int> n_out;            // New size of the array
    sc_out<int> arr_out[9];       // Output array (fixed size for simplicity)

    SC_CTOR(RemoveDuplicates) {
        SC_METHOD(remove_duplicates);
        sensitive << clk.posedge_event() << rst;
    }

    void remove_duplicates() {
        if (rst) {
            n_out.write(0);
            for (int i = 0; i < 9; ++i) {
                arr_out[i].write(0);
            }
        } else {
            int temp[9];
            int j = 0;

            if (n.read() == 0 || n.read() == 1) {
                n_out.write(n.read());
                for (int i = 0; i < n.read(); ++i) {
                    arr_out[i].write(arr_in[i].read());
                }
            } else {
                for (int i = 0; i < n.read() - 1; ++i) {
                    if (arr_in[i].read() != arr_in[i + 1].read()) {
                        temp[j++] = arr_in[i].read();
                    }
                }
                temp[j++] = arr_in[n.read() - 1].read();

                n_out.write(j);
                for (int i = 0; i < j; ++i) {
                    arr_out[i].write(temp[i]);
                }
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;          // Clock signal
    sc_signal<bool> rst;          // Reset signal
    sc_signal<int> n;             // Size of the array
    sc_signal<int> arr_in[9];      // Input array (fixed size for simplicity)
    sc_signal<int> n_out;          // New size of the array
    sc_signal<int> arr_out[9];     // Output array (fixed size for simplicity)

    RemoveDuplicates remove_duplicates_inst;

    SC_CTOR(Testbench) : remove_duplicates_inst("remove_duplicates_inst") {
        // Connect signals to ports
        remove_duplicates_inst.clk(clk);
        remove_duplicates_inst.rst(rst);
        remove_duplicates_inst.n(n);
        for (int i = 0; i < 9; ++i) {
            remove_duplicates_inst.arr_in[i](arr_in[i]);
            remove_duplicates_inst.arr_out[i](arr_out[i]);
        }
        remove_duplicates_inst.n_out(n_out);

        // Process to run tests
        SC_THREAD(run_tests);
        sensitive << clk.posedge_event();
    }

    void run_tests() {
        // Initialize clock and reset
        clk = 0;
        rst = 1;
        wait(1, SC_NS);
        rst = 0;
        wait(1, SC_NS);

        // Set input array and size
        n = 9;
        arr_in[0] = 1; arr_in[1] = 2; arr_in[2] = 2; arr_in[3] = 3;
        arr_in[4] = 4; arr_in[5] = 4; arr_in[6] = 4; arr_in[7] = 5;
        arr_in[8] = 5;

        // Wait for the operation to complete
        wait(1, SC_NS);

        // Print updated array
        for (int i = 0; i < n_out.read(); ++i) {
            cout << arr_out[i].read() << " ";
        }
        cout << endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
