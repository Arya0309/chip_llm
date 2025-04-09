
#include <systemc.h>
#include <cmath>

// Module to check if a prime number can be expressed as sum of two prime numbers

SC_MODULE(PrimeSumChecker) {
    sc_in<int> n_in;       // Input port for the number to check
    sc_out<bool> result_out; // Output port for the result

    // Constructor
    SC_CTOR(PrimeSumChecker) {
        // Process to check if the number can be expressed as sum of two primes
        SC_METHOD(check_sum_of_primes);
        sensitive << n_in;
    }

    // Method to check if a number is prime
    bool isPrime(int n) {
        if (n <= 1)
            return false;
        for (int i = 2; i <= std::sqrt(n); i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }

    // Method to check if a prime number can be expressed as sum of two prime numbers
    void check_sum_of_primes() {
        int N = n_in.read();
        bool result = false;
        if (isPrime(N) && isPrime(N - 2))
            result = true;
        result_out.write(result);
    }
};

// Example top module for testing

SC_MODULE(Top) {
    PrimeSumChecker checker;
    sc_signal<int> n_in;
    sc_signal<bool> result_out;

    SC_CTOR(Top) : checker("checker") {
        checker.n_in(n_in);
        checker.result_out(result_out);

        // Initialize and run simulation
        SC_THREAD(test);
    }

    void test() {
        n_in.write(13); // Test with number 13
        wait(1, SC_NS);
        cout << "For n = 13, Result: " << (result_out.read() ? "Yes" : "No") << endl;

        n_in.write(17); // Additional test with number 17
        wait(1, SC_NS);
        cout << "For n = 17, Result: " << (result_out.read() ? "Yes" : "No") << endl;

        sc_stop(); // Stop the simulation
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
