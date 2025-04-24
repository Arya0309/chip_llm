c
#include <systemc.h>

// Module that calculates whether a number is an Armstrong number or not

SC_MODULE(ArmstrongChecker) {
    sc_in<int> input_num;  // Input port for the number
    sc_out<bool> output_result;  // Output port for the result

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(process_input);
        sensitive << input_num;
    }

    // Process to check whether a number is an Armstrong number or not
    void process_input() {
        int n = input_num.read();
        int sum = 0;
        if (n > 0) {  // Check if the input number is non-zero
            int temp = n;
            while (n > 0) {
                int rem = n % 10;
                int cube = rem * rem * rem;
                sum += cube;
                n /= 10;
            }
            if (temp == sum) {  // Check if the sum of cubes is equal to the input number
                output_result.write(true);  // Output TRUE if the number is an Armstrong number
            } else {
                output_result.write(false);  // Output FALSE if the number is not an Armstrong number
            }
        } else {
            output_result.write(false);  // Output FALSE if the input number is 0
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
