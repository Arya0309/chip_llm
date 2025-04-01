
#include <systemc.h>

// Module to check if a number is an Armstrong number
SC_MODULE(ArmstrongChecker) {
    sc_in<int> number;          // Input port for the number to check
    sc_out<bool> is_armstrong;  // Output port to indicate if the number is an Armstrong number

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Process to check Armstrong number
        SC_METHOD(check_armstrong);
        sensitive << number;
    }

    // Method to check if the number is an Armstrong number
    void check_armstrong() {
        int n = number.read();
        int temp = n;
        int p = 0;

        while (n > 0) {
            int rem = n % 10;
            p = (p) + (rem * rem * rem);
            n = n / 10;
        }

        if (temp == p) {
            is_armstrong.write(true);
        } else {
            is_armstrong.write(false);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number;          // Signal for the number to check
    sc_signal<bool> is_armstrong;  // Signal to receive the result

    ArmstrongChecker checker_inst; // Instance of the ArmstrongChecker module

    // Constructor
    SC_CTOR(Testbench) : checker_inst("checker") {
        // Connect signals to ports
        checker_inst.number(number);
        checker_inst.is_armstrong(is_armstrong);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Number is 153
        number = 153;
        wait(1, SC_NS); // Wait for the checker to process
        assert(is_armstrong.read() == true);
        cout << "Number 153 is an Armstrong Number: " << (is_armstrong.read() ? "Yes" : "No") << endl;

        // Test case 2: Number is 123
        number = 123;
        wait(1, SC_NS);
        assert(is_armstrong.read() == false);
        cout << "Number 123 is an Armstrong Number: " << (is_armstrong.read() ? "Yes" : "No") << endl;

        // Additional test case: Number is 370
        number = 370;
        wait(1, SC_NS);
        assert(is_armstrong.read() == true);
        cout << "Number 370 is an Armstrong Number: " << (is_armstrong.read() ? "Yes" : "No") << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
