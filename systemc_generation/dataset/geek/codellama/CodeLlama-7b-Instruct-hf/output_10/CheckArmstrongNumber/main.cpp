scss
#include <systemc.h>

// Module to test whether a number is Armstrong or not

SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in;  // Input port for the number to be tested
    sc_out<bool> is_armstrong;  // Output port indicating Armstrong-ness

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Define a temporary variable for intermediate calculation
        sc_signal<int> temp;
        sc_signal<int> p;

        // Process to calculate the sum of cubes
        SC_METHOD(calculate_sum);
        sensitive << num_in;

        // Condition to check Armstrong-ness
        SC_METHOD(check_armstrong);
        sensitive << temp << p;
    }

    // Method to calculate the sum of cubes
    void calculate_sum() {
        int n = num_in.read();
        int rem = 0;
        int p = 0;

        while (n > 0) {
            rem = n % 10;
            p = (p) + (rem * rem * rem);
            n = n / 10;
        }

        temp.write(p);
    }

    // Method to check Armstrong-ness
    void check_armstrong() {
        int temp = temp.read();
        int p = p.read();

        if (temp == p) {
            is_armstrong.write(true);
        } else {
            is_armstrong.write(false);
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
