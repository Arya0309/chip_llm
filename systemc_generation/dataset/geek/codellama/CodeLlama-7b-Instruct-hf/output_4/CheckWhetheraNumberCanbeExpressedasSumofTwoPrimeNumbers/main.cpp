
#include <systemc.h>

// Module that checks whether a number can be expressed as the sum of two prime numbers

SC_MODULE(PrimeSumChecker) {
    sc_in<int> n_in;  // Input port for the number
    sc_out<bool> result_out; // Output port for the result

    // Function to check whether a number is prime
    const int MAX_PRIME = 100; // Maximum prime number checked
    bool isPrime(int n) {
        if (n <= 1)
            return false;  // 1 is not prime
        for (int i = 2; i <= sqrt(n); i++)
            if (n % i == 0)
                return false;  // If the number is divisible by any number between 2 and sqrt(n), it is not prime
        return true;  // If no divisors are found, the number is prime
    }

    // Function to check if a number can be expressed as the sum of two prime numbers
    bool isPossible(int N) {
        if (isPrime(N) && isPrime(N - 2))
            return true;  // If the number and its difference of two are both prime, it can be expressed as the sum of two prime numbers
        else
            return false;  // Otherwise, it cannot be expressed
    }

    // Constructor
    SC_CTOR(PrimeSumChecker) {
        // Call the isPossible function and print the result
        SC_METHOD(check);
        sensitive << n_in;
    }

    // Method to check the result
    void check() {
        result_out = isPossible(n_in.read());
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
