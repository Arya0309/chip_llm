scss
// SystemC code for Armstrong number checker

#include <systemc.h>

// Input port for user input
sc_in<int> num_in;

// Output port to indicate whether the number is an Armstrong number or not
sc_out<bool> is_armstrong;

// Module to check if a number is an Armstrong number or not

SC_MODULE(ArmstrongChecker) {
    SC_CTOR(ArmstrongChecker) {
        // Process to calculate the sum of the cubes of the digits of num_in
        SC_METHOD(calculate_sum);
        sensitive << num_in;

        // Process to check if the sum is equal to num_in
        SC_METHOD(check_sum);
        sensitive << num_in;

        // Combine the two processes to create a complete checker
        is_armstrong = calculate_sum();
        is_armstrong = check_sum(is_armstrong);
    }

    // Function to calculate the sum of the cubes of the digits of num_in
    int calculate_sum() {
        int n = num_in;
        int temp = n;
        int p = 0;

        // Loop to calculate the sum of the cubes of the digits of n
        while (n > 0) {
            int rem = n % 10;
            p = (p) + (rem * rem * rem);
            n = n / 10;
        }

        return p;
    }

    // Function to check if the sum is equal to num_in
    bool check_sum(bool is_armstrong) {
        if (num_in == is_armstrong) {
            return 1;
        }
        else {
            return 0;
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
