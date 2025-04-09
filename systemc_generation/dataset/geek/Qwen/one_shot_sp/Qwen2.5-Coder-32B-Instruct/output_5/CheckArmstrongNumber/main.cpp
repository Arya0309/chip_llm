
#include <systemc.h>

// Module to check if a number is an Armstrong number

SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in;          // Input port for the number to check
    sc_out<bool> is_armstrong;  // Output port indicating if the number is an Armstrong number

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Process to check if the number is an Armstrong number
        SC_METHOD(check_armstrong);
        sensitive << num_in;
    }

    // Method to determine if the number is an Armstrong number
    void check_armstrong() {
        int n = num_in.read();  // Read the input number
        int temp = n;           // Store the original number for comparison
        int p = 0;              // Initialize the sum of cubes

        // Calculate the sum of cubes of the digits
        while (n > 0) {
            int rem = n % 10;   // Extract the last digit
            p += rem * rem * rem; // Add the cube of the digit to p
            n /= 10;            // Remove the last digit
        }

        // Determine if the number is an Armstrong number
        bool result = (temp == p);

        // Write the result to the output port
        is_armstrong.write(result);
    }
};

// Example top module to instantiate the ArmstrongChecker

SC_MODULE(TopModule) {
    sc_signal<int> num_in;          // Signal for the input number
    sc_signal<bool> is_armstrong;  // Signal for the output result

    ArmstrongChecker checker;       // Instantiate the ArmstrongChecker module

    // Constructor
    SC_CTOR(TopModule) : checker("checker") {
        // Connect the signals to the checker module
        checker.num_in(num_in);
        checker.is_armstrong(is_armstrong);

        // Initialization process to test the module
        SC_THREAD(test);
    }

    // Test thread to drive the input and observe the output
    void test() {
        num_in = 153;  // Test with the number 153
        wait(1, SC_NS);  // Wait for one nanosecond to see the result
        cout << "Number: " << num_in.read() << " is Armstrong: " << is_armstrong.read() << endl;

        num_in = 123;  // Test with another number
        wait(1, SC_NS);  // Wait for one nanosecond to see the result
        cout << "Number: " << num_in.read() << " is Armstrong: " << is_armstrong.read() << endl;

        sc_stop();  // Stop the simulation
    }
};

// Main function to start the simulation


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
