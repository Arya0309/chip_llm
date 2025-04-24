scss
#include <systemc.h>

// Simple trial division module

SC_MODULE(PrimeChecker) {
    // Declare input and output ports
    sc_in<int> n;
    sc_out<bool> is_prime;

    // Constructor
    SC_CTOR(PrimeChecker) {
        // Define processes
        SC_METHOD(divide);
        sensitive << n;
        SC_METHOD(check_prime);
        sensitive << is_prime;
    }

    // Process to check divisibility
    void divide() {
        // Initialize counter
        int cnt = 0;

        // Check for divisors from 1 to n
        for (int i = 1; i <= n; i++) {
            // Increment counter if n is divisible by i
            if (n % i == 0)
                cnt++;
        }
    }

    // Process to check for primality
    void check_prime() {
        // If n is divisible by more than 2 numbers
        // then it is not prime
        if (cnt > 2) {
            is_prime.write(0);
        } else {
            // Otherwise it is prime
            is_prime.write(1);
        }
    }
};

// Main module

SC_MODULE(main) {
    sc_in<int> n;
    sc_out<bool> is_prime;

    PrimeChecker PrimeChecker_module;
    SC_CTOR(main) {
        PrimeChecker_module.n(n);
        PrimeChecker_module.is_prime(is_prime);
    }
};



SC_MODULE(Testbench) {
    sc_signal<int> n_sig;       // Signal for the input number
    sc_signal<bool> prime_sig;  // Signal for the prime check result

    // Instance of the PrimeChecker module
    PrimeChecker prime_checker_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : prime_checker_inst("prime_checker_inst") {
        // Connect signals to module ports
        prime_checker_inst.n(n_sig);
        prime_checker_inst.is_prime(prime_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 29 (expected: prime)
        n_sig.write(29);
        wait(1, SC_NS); // Wait for the process to update
        bool result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == true);

        // Test case: n = 1 (expected: NOT prime)
        n_sig.write(1);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == false);

        // Test case: n = 10 (expected: NOT prime)
        n_sig.write(10);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == false);

        // Test case: n = 13 (expected: prime)
        n_sig.write(13);
        wait(1, SC_NS);
        result = prime_sig.read();
        std::cout << "Number " << n_sig.read() << " is " << (result ? "prime" : "NOT prime") << std::endl;
        assert(result == true);

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
