
#include <systemc.h>

// Module that sorts an array using bubble sort
SC_MODULE(Sorter) {
    sc_in<bool> clk;  // Clock signal
    sc_in<bool> reset;  // Reset signal
    sc_in<sc_uint<8>> input[5];  // Input array
    sc_out<sc_uint<8>> output[5];  // Output array
    sc_out<bool> done;  // Signal indicating sorting completion

    sc_uint<8> nums[5];  // Internal storage for the array
    bool isSwapped;  // Flag to check if any swapping occurred

    // Constructor
    SC_CTOR(Sorter) {
        // Process to perform sorting
        SC_METHOD(sort_array);
        sensitive << clk.posedge_event() << reset.posedge_event();
    }

    // Method to sort the array
    void sort_array() {
        if (reset.read()) {
            // Initialize internal array and output array
            for (int i = 0; i < 5; i++) {
                nums[i] = input[i].read();
                output[i].write(nums[i]);
            }
            done.write(false);
        } else if (!done.read()) {
            static int i = 0, j = 1;
            isSwapped = false;

            if (i < 5) {
                if (j < 5 - i) {
                    if (nums[j] < nums[j - 1]) {
                        // Swap elements
                        sc_uint<8> temp = nums[j];
                        nums[j] = nums[j - 1];
                        nums[j - 1] = temp;
                        isSwapped = true;
                    }
                    j++;
                } else {
                    if (!isSwapped) {
                        done.write(true);
                    }
                    i++;
                    j = 1;
                }
            }

            // Write sorted array to output
            for (int k = 0; k < 5; k++) {
                output[k].write(nums[k]);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;  // Clock signal
    sc_signal<bool> reset;  // Reset signal
    sc_signal<sc_uint<8>> input[5];  // Input array
    sc_signal<sc_uint<8>> output[5];  // Output array
    sc_signal<bool> done;  // Signal indicating sorting completion

    Sorter sorter_inst; // Instance of the Sorter module

    // Constructor
    SC_CTOR(Testbench) : sorter_inst("sorter") {
        // Connect signals to ports
        sorter_inst.clk(clk);
        sorter_inst.reset(reset);
        for (int i = 0; i < 5; i++) {
            sorter_inst.input[i](input[i]);
            sorter_inst.output[i](output[i]);
        }
        sorter_inst.done(done);

        // Process to generate clock
        SC_THREAD(generate_clock);
        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to generate clock
    void generate_clock() {
        while (true) {
            clk.write(false);
            wait(5, SC_NS);
            clk.write(true);
            wait(5, SC_NS);
        }
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize inputs
        input[0] = 1;
        input[1] = 12;
        input[2] = 6;
        input[3] = 8;
        input[4] = 10;

        // Assert reset
        reset.write(true);
        wait(10, SC_NS);
        reset.write(false);

        // Wait until sorting is done
        while (!done.read()) {
            wait(10, SC_NS);
        }

        // Print the sorted array
        cout << "After sorting the array is: \n";
        for (int i = 0; i < 5; i++) {
            cout << output[i].read() << " ";
        }
        cout << "\n";

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
