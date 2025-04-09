
#include <systemc.h>

// Define constants
static const int ARRAY_SIZE = 7;

// Module that rotates an array

SC_MODULE(Rotator) {
    // Clock and reset signals
    sc_in<bool> clk;
    sc_in<bool> rst;
    
    // Input signal for the number of positions to rotate
    sc_in<int> d;
    
    // Input and output arrays
    sc_in<int> in_array[ARRAY_SIZE];
    sc_out<int> out_array[ARRAY_SIZE];
    
    // Internal storage for the array during rotation
    int temp[ARRAY_SIZE];
    
    // Constructor
    SC_CTOR(Rotator) {
        // Sensitivity list: react to rising edge of clock or positive edge of reset
        SC_METHOD(rotate_array);
        sensitive << clk.pos() << rst.pos();
    }
    
    // Method to rotate the array
    void rotate_array() {
        if (rst.read()) {
            // Reset condition: initialize output array to zero or any default value
            for (int i = 0; i < ARRAY_SIZE; ++i) {
                out_array[i].write(0);
            }
        } else {
            // Read input array into a local buffer
            for (int i = 0; i < ARRAY_SIZE; ++i) {
                temp[i] = in_array[i].read();
            }
            
            // Perform rotation
            int shift = d.read() % ARRAY_SIZE; // Ensure shift is within array bounds
            int k = 0;
            
            // Store elements from index 'shift' to end of array in temp
            for (int i = shift; i < ARRAY_SIZE; ++i) {
                temp[k++] = in_array[i].read();
            }
            
            // Store elements from start of array to index 'shift-1' in temp
            for (int i = 0; i < shift; ++i) {
                temp[k++] = in_array[i].read();
            }
            
            // Write rotated array to output
            for (int i = 0; i < ARRAY_SIZE; ++i) {
                out_array[i].write(temp[i]);
            }
        }
    }
};

// Example usage


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
