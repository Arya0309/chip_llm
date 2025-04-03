
#include <systemc.h>

// Module to remove consecutive duplicates
SC_MODULE(RemoveDuplicates) {
    sc_in<bool> clk;             // Clock signal
    sc_in<bool> reset;           // Reset signal
    sc_vector<sc_in<int>> input;  // Input array
    sc_vector<sc_out<int>> output;// Output array
    sc_out<int> new_size;         // New size of the array after removing duplicates

    int temp[9];                 // Temporary storage for unique elements
    int j;                       // Index for unique elements

    // Constructor
    SC_CTOR(RemoveDuplicates) : input("input", 9), output("output", 9) {
        SC_METHOD(process);
        sensitive << clk.pos();
    }

    // Process to remove duplicates
    void process() {
        if (reset.read()) {
            j = 0;
            new_size.write(0);
            for (int i = 0; i < 9; i++) {
                output[i].write(0);
            }
        } else {
            // Edge case handling
            if (input.size() == 0 || input.size() == 1) {
                new_size.write(input.size());
                for (int i = 0; i < input.size(); i++) {
                    output[i].write(input[i].read());
                }
            } else {
                // Initialize j
                j = 0;

                // Loop to remove duplicates
                for (int i = 0; i < input.size() - 1; i++) {
                    if (input[i].read() != input[i + 1].read()) {
                        temp[j++] = input[i].read();
                    }
                }

                // Store last element
                temp[j++] = input[input.size() - 1].read();

                // Copy unique elements back to output
                for (int i = 0; i < j; i++) {
                    output[i].write(temp[i]);
                }

                // Write new size
                new_size.write(j);
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<bool> clk;          // Clock signal
    sc_signal<bool> reset;        // Reset signal
    sc_vector<sc_signal<int>> input;// Input array
    sc_vector<sc_signal<int>> output;// Output array
    sc_signal<int> new_size;       // New size of the array

    RemoveDuplicates remove_duplicates_inst; // Instance of the RemoveDuplicates module

    // Constructor
    SC_CTOR(Testbench) : input("input", 9), output("output", 9), remove_duplicates_inst("remove_duplicates") {
        // Connect signals to ports
        remove_duplicates_inst.clk(clk);
        remove_duplicates_inst.reset(reset);
        for (int i = 0; i < 9; i++) {
            remove_duplicates_inst.input[i](input[i]);
            remove_duplicates_inst.output[i](output[i]);
        }
        remove_duplicates_inst.new_size(new_size);

        // Process to generate clock and reset
        SC_THREAD(generate_clock);
        SC_THREAD(generate_reset);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to generate clock
    void generate_clock() {
        while (true) {
            clk.write(false);
            wait(1, SC_NS);
            clk.write(true);
            wait(1, SC_NS);
        }
    }

    // Thread to generate reset
    void generate_reset() {
        reset.write(true);
        wait(2, SC_NS);
        reset.write(false);
        wait(SC_ZERO_TIME);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize input array
        int arr[] = {1, 2, 2, 3, 4, 4, 4, 5, 5};
        for (int i = 0; i < 9; i++) {
            input[i].write(arr[i]);
        }

        // Wait for processing
        wait(5, SC_NS);

        // Print updated array
        cout << "Updated array: ";
        for (int i = 0; i < new_size.read(); i++) {
            cout << output[i].read() << " ";
        }
        cout << endl;

        // Print new size
        cout << "New size: " << new_size.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
