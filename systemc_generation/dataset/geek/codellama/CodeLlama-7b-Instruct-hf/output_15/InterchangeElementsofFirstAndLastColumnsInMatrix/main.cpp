scss
#include <systemc.h>

// Define the module to add two integers

SC_MODULE(MatrixSwapper) {
    sc_in<int> in[SIZE];  // Input ports for the two integers to add
    sc_out<int> out[SIZE]; // Output port for the sum

    // Constructor to connect the inputs and outputs
    SC_CTOR(MatrixSwapper) {
        // Connect the inputs and outputs of the module
        SC_METHOD(add);
        sensitive << in[0] << in[1] << out[0];
    }

    // Method to add the two integers
    void add() {
        // Calculate the sum of the two integers
        int sum = in[0].read() + in[1].read();
        // Write the sum to the output port
        out[0].write(sum);
    }
};

// Define the testbench module

SC_MODULE(TB) {
    sc_in<int> in[SIZE]; // Input ports for the two integers to add
    sc_out<int> out[SIZE]; // Output port for the sum

    // Instantiate the adder module
    MatrixSwapper adder("adder");

    // Connect the input and output ports of the adder module with the testbench
    adder.in[0] = in[0];
    adder.in[1] = in[1];
    adder.out[0] = out[0];

    // Define the testbench functionality
    void test() {
        // Initialize the input values
        in[0] = 8;  // Row 0, Col 0
        in[1] = 9;  // Row 0, Col 1
        in[3] = 7;  // Row 1, Col 0
        in[4] = 6;  // Row 1, Col 1
        in[6] = 5;  // Row 2, Col 0
        in[7] = 4;  // Row 2, Col 1
        in[8] = 3;  // Row 3, Col 0
        in[9] = 2;  // Row 3, Col 1
        in[10] = 1; // Row 4, Col 0
        in[11] = 8; // Row 4, Col 1
        
        // Call the adder module
        adder.tick();
        
        // Check the output values
        if (out[0].read() != 3) {
            printf("Output did not match expected value\n");
            exit(1);
        }
    }

    // Tick the testbench
    SC_CTHREAD(test, clk.pos());
    async_reset_signal_is(rst, false);

    // Define the clock and reset signals
    sc_clock clk("clock", 10, SC_NS);
    sc_signal<bool> rst;
};



SC_MODULE(Testbench) {
    // Use static const for constants instead of #define
    static const int N = 4;
    static const int SIZE = N * N;
    
    sc_signal<int> in[SIZE];
    sc_signal<int> out[SIZE];
    MatrixSwapper* swapper;

    SC_CTOR(Testbench) {
        swapper = new MatrixSwapper("swapper");
        // Connect the signals to the MatrixSwapper ports
        for (int i = 0; i < SIZE; i++) {
            swapper->in[i](in[i]);
            swapper->out[i](out[i]);
        }
        SC_THREAD(run_tests);
    }

    // Thread that runs the test, checks outputs, and asserts correctness
    void run_tests() {
        // Initialize the matrix as in the original C++ code:
        //   { {8, 9, 7, 6},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {9, 9, 7, 7} }
        int m[SIZE] = {8, 9, 7, 6,
                       4, 7, 6, 5,
                       3, 2, 1, 8,
                       9, 9, 7, 7};
        for (int i = 0; i < SIZE; i++) {
            in[i].write(m[i]);
        }

        // Wait for signals to propagate
        wait(1, SC_NS);

        // Expected output after swapping first and last rows:
        //   { {9, 9, 7, 7},
        //     {4, 7, 6, 5},
        //     {3, 2, 1, 8},
        //     {8, 9, 7, 6} }
        int expected[SIZE] = {9, 9, 7, 7,
                              4, 7, 6, 5,
                              3, 2, 1, 8,
                              8, 9, 7, 6};

        // Print and check the result matrix
        std::cout << "Swapped Matrix:" << std::endl;
        for (int row = 0; row < N; row++) {
            for (int col = 0; col < N; col++) {
                int idx = row * N + col;
                int result = out[idx].read();
                std::cout << result << " ";
                assert(result == expected[idx]);
            }
            std::cout << std::endl;
        }
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench
    sc_start();         // Start the simulation
    return 0;
}
