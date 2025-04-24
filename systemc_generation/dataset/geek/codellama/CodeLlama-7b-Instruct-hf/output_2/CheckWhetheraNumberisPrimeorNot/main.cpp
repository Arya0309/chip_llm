
#include <systemc.h>

// Module that adds two integers

SC_MODULE(PrimeChecker) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> c; // Output port for the sum

    // Constructor
    SC_CTOR(PrimeChecker) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to add the two numbers
    void add() {
        c.write(a.read() + b.read());
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
