c
#include <systemc.h>

// Module that calculates an Armstrong number

SC_MODULE(ArmstrongChecker) {
    sc_in<int> input; // Input port for the number to be checked
    sc_out<bool> output; // Output port for the result (true or false)

    int temp; // Temporary variable to store the input value
    int p; // Variable to store the sum of digits raised to the power of their positions

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Process to calculate Armstrong number
        SC_METHOD(calculate);
        sensitive << input;
    }

    // Method to calculate the Armstrong number
    void calculate() {
        temp = input.read();
        p = 0;

        // Loop to calculate the sum of digits raised to the power of their positions
        while (temp > 0) {
            int rem = temp % 10;
            p += (rem * rem * rem);
            temp /= 10;
        }

        // Condition to check whether the input number is an Armstrong number or not
        if (temp == p) {
            output.write(true);
        }
        else {
            output.write(false);
        }
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> num_sig;       // Signal for the number input
    sc_signal<bool> armstrong_sig; // Signal for the Armstrong check result

    // Instance of the ArmstrongChecker module
    ArmstrongChecker armstrong_checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : armstrong_checker_inst("armstrong_checker_inst") {
        // Connect signals to module ports
        armstrong_checker_inst.num_in(num_sig);
        armstrong_checker_inst.is_armstrong(armstrong_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 153 (expected: Armstrong number)
        num_sig.write(153);
        wait(1, SC_NS); // Wait for the process to update
        bool result = armstrong_sig.read();
        std::cout << "Number: 153 is Armstrong? " << (result ? "Yes" : "No") << std::endl;
        assert(result == true);

        // Test case: n = 123 (expected: Not an Armstrong number)
        num_sig.write(123);
        wait(1, SC_NS); // Wait for the process to update
        result = armstrong_sig.read();
        std::cout << "Number: 123 is Armstrong? " << (result ? "Yes" : "No") << std::endl;
        assert(result == false);

        // End simulation after successful test
        std::cout << "Tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
