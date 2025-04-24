
#include <systemc.h>


SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in; // Input port for the number to be checked
    sc_out<bool> is_armstrong; // Output port for whether the number is an ArmstrongChecker number or not

    void calc_is_armstrong() {
        int n = num_in.read(); // Read the input number
        int temp = n; // Make a copy of the input number
        int p = 0; // Initialize the sum variable
        while (n > 0) { // While the input number is not equal to 0
            int rem = n % 10; // Get the current digit
            p = p + (rem * rem * rem); // Calculate the cube of the current digit and add it to the sum
            n = n / 10; // Divide the input number by 10
        }
        if (temp == p) { // If the sum is equal to the original input number
            is_armstrong.write(true); // Write true to the output port
        } else { // Otherwise
            is_armstrong.write(false); // Write false to the output port
        }
    }

    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(calc_is_armstrong);
        sensitive << num_in; // Register the input port as a sensitive signal
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
