
#include <systemc.h>


SC_MODULE(PrimeSumChecker) {
    sc_in<int> n_in;  // Input port for the number
    sc_out<bool> result_out; // Output port for the result

    // Constructor
    SC_CTOR(PrimeSumChecker) {
        // Process to determine if the number can be expressed as the sum of two primes
        SC_METHOD(check_prime_sum);
        sensitive << n_in;
    }

    // Method to check if a number is prime
    bool isPrime(int n) {
        if (n <= 1)
            return false;

        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0)
                return false;
        }

        return true;
    }

    // Method to check if a prime number can be expressed as the sum of two prime numbers
    bool isPossible(int N) {
        if (isPrime(N) && isPrime(N - 2))
            return true;
        else
            return false;
    }

    // Method to process the input and produce the output
    void check_prime_sum() {
        int n = n_in.read();
        bool result = isPossible(n);
        result_out.write(result);
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
