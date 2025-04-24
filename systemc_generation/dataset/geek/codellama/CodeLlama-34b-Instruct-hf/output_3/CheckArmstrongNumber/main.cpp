scss
#ifndef ARMSTRONG_NUMBER_H_
#define ARMSTRONG_NUMBER_H_

#include <systemc.h>

// Module that checks if an input number is an Armstrong number

SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in;  // Input port for the input number
    sc_out<bool> is_armstrong;  // Output port for the result

    // Process to check if the input number is an Armstrong number
    SC_CTOR(ArmstrongChecker) {
        // Call the method to perform the check
        SC_METHOD(checkArmstrongNumber);
        sensitive << num_in;
    }

    // Method to check if the input number is an Armstrong number
    void checkArmstrongNumber() {
        int n = num_in.read();  // Read the input number
        int p = 0;  // Initialize the sum of the powers of the digits to 0
        while (n > 0) {
            int digit = n % 10;  // Get the last digit of n
            p += digit * digit * digit;  // Calculate the sum of the powers of the digits
            n /= 10;  // Remove the last digit of n
        }
        is_armstrong.write(p == num_in.read());  // Set the output signal accordingly
    }
};

#endif /* ARMSTRONG_NUMBER_H_ */


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
