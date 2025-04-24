
#include <systemc.h>


SC_MODULE(PrimeSumChecker) {
    sc_in<int> n_in;
    sc_out<bool> result_out;

    SC_CTOR(PrimeSumChecker) {
        // Process to check if n_in is prime
        SC_METHOD(check_prime);
        sensitive << n_in;
        // Process to check if n_in can be expressed as the sum of two prime numbers
        SC_METHOD(check_possible);
        sensitive << n_in;
    }

    // Method to check if n_in is prime
    void check_prime() {
        if (n_in.read() <= 1) {
            result_out.write(false);
            return;
        }
        for (int i = 2; i <= n_in.read(); i++) {
            if (n_in.read() % i == 0) {
                result_out.write(false);
                return;
            }
        }
        result_out.write(true);
    }

    // Method to check if n_in can be expressed as the sum of two prime numbers
    void check_possible() {
        if (result_out.read() == true && n_in.read() - 2 >= 2) {
            result_out.write(true);
        } else {
            result_out.write(false);
        }
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> n_sig;       // Signal for the input number
    sc_signal<bool> result_sig; // Signal for the result

    // Instance of the PrimeSumChecker module
    PrimeSumChecker checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : checker_inst("checker_inst") {
        // Connect signals to module ports
        checker_inst.n_in(n_sig);
        checker_inst.result_out(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 13 (expected result: true)
        n_sig.write(13);
        wait(1, SC_NS); // Wait for the process to update
        bool result = result_sig.read();
        std::cout << "Can 13 be expressed as the sum of two primes? " << (result ? "Yes" : "No") << std::endl;
        assert(result == true);

        // Additional test case: n = 17 (expected result: false)
        n_sig.write(17);
        wait(1, SC_NS);
        result = result_sig.read();
        std::cout << "Can 17 be expressed as the sum of two primes? " << (result ? "Yes" : "No") << std::endl;
        assert(result == false);

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
