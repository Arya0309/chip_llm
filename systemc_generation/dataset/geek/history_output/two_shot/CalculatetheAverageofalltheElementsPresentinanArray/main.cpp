
#include <systemc.h>

// Module to calculate the average of an array
SC_MODULE(AverageCalculator) {
    sc_in<bool> clk;          // Clock signal
    sc_in<bool> rst;          // Reset signal
    sc_in<int> arr[9];        // Input array (fixed size for simplicity)
    sc_out<double> avg;       // Output average

    // Internal variables
    int sum;
    int count;
    bool ready;

    // Constructor
    SC_CTOR(AverageCalculator) {
        // Initialize internal variables
        sum = 0;
        count = 0;
        ready = false;

        // Sensitivity list for the process
        SC_METHOD(calculate_average);
        sensitive << clk.pos();
        dont_initialize();
    }

    // Method to calculate the average
    void calculate_average() {
        if (rst.read()) {
            // Reset internal state
            sum = 0;
            count = 0;
            ready = false;
        } else {
            // Accumulate sum and count elements
            sum += arr[count].read();
            count++;

            // Check if all elements have been processed
            if (count == 9) {
                // Calculate average and set ready flag
                avg.write((double)sum / 9);
                ready = true;
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk;             // Clock signal
    sc_signal<bool> rst;      // Reset signal
    sc_signal<int> arr[9];    // Array of input signals
    sc_signal<double> avg;    // Average output signal

    // Instance of the AverageCalculator module
    AverageCalculator avg_inst;

    // Constructor
    SC_CTOR(Testbench)
    : avg_inst("avg_inst"), clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true) {
        // Connect signals to module ports
        avg_inst.clk(clk);
        avg_inst.rst(rst);
        for (int i = 0; i < 9; i++) {
            avg_inst.arr[i](arr[i]);
        }
        avg_inst.avg(avg);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        rst.write(true);
        wait(10, SC_NS);
        rst.write(false);

        // Initialize array values
        for (int i = 0; i < 9; i++) {
            arr[i].write(i + 10); // Values 10, 11, ..., 18
        }

        // Wait for the calculation to complete
        wait(100, SC_NS);

        // Print the result
        cout << "Average: " << avg.read() << endl;

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
