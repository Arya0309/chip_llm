
#include <systemc.h>

// Module to determine if a number is Even or Odd
SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in;          // Input port for the number to check
    sc_out<sc_bv<4>> result_out;   // Output port for the result (0000 for Even, 0001 for Odd)

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Process to check if the number is even or odd
        SC_METHOD(check_even_odd);
        sensitive << number_in;
    }

    // Method to check if the number is even or odd
    void check_even_odd() {
        int num = number_in.read();
        if (num % 2 == 0) {
            result_out.write(0); // Write 0 for Even
        } else {
            result_out.write(1); // Write 1 for Odd
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number_sig;       // Signal for the input number
    sc_signal<sc_bv<4>> result_sig;  // Signal for the result (Even/Odd)

    // Instance of the EvenOddChecker module
    EvenOddChecker checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : checker_inst("checker_inst") {
        // Connect signals to module ports
        checker_inst.number_in(number_sig);
        checker_inst.result_out(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Number is 11 (Odd)
        number_sig.write(11);
        wait(1, SC_NS); // Wait for the process to update
        int result = result_sig.read().to_uint();
        std::cout << "Number: " << number_sig.read() << " is " << (result == 0 ? "Even" : "Odd") << std::endl;
        assert(result == 1);

        // Test case 2: Number is 12 (Even)
        number_sig.write(12);
        wait(1, SC_NS); // Wait for the process to update
        result = result_sig.read().to_uint();
        std::cout << "Number: " << number_sig.read() << " is " << (result == 0 ? "Even" : "Odd") << std::endl;
        assert(result == 0);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
