
#include <systemc.h>


SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in;   // input port for the number to be checked
    sc_out<bool> is_armstrong;  // output port for whether the number is an Armstrong number or not

    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(check_armstrong);
        sensitive << num_in;
    }

    void check_armstrong() {
        int n = num_in.read();  // read the input number from the input port
        int temp = n;
        int p = 0;

        while (n > 0) {
            int rem = n % 10;  // calculate the last digit of the number
            p = (p) + (rem * rem * rem);  // calculate the sum of the cubes of the digits
            n = n / 10;  // subtract the last digit and divide by 10
        }

        if (temp == p) {
            is_armstrong.write(true);  // write true to the output port if the number is an Armstrong number
        } else {
            is_armstrong.write(false);  // write false to the output port if the number is not an Armstrong number
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
