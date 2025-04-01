
#include <systemc.h>

// Module to check for prime number using Simple Trial Division
SC_MODULE(PrimeChecker) {
    sc_in<int> n;  // Input port for the number to check
    sc_out<bool> is_prime; // Output port indicating if the number is prime

    // Constructor
    SC_CTOR(PrimeChecker) {
        // Process to check for primality
        SC_METHOD(check_prime);
        sensitive << n;
    }

    // Method to check if the number is prime
    void check_prime() {
        int cnt = 0;
        int num = n.read();
        
        if (num <= 1) {
            is_prime.write(false);
            return;
        }

        for (int i = 1; i <= num; i++) {
            if (num % i == 0)
                cnt++;
        }

        if (cnt > 2)
            is_prime.write(false);
        else
            is_prime.write(true);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n; // Signal for the number to check
    sc_signal<bool> is_prime; // Signal for the result

    PrimeChecker prime_checker_inst; // Instance of the PrimeChecker module

    // Constructor
    SC_CTOR(Testbench) : prime_checker_inst("prime_checker") {
        // Connect signals to ports
        prime_checker_inst.n(n);
        prime_checker_inst.is_prime(is_prime);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Prime number
        n = 29;
        wait(1, SC_NS); // Wait for the prime checker to process
        assert(is_prime.read() == true);
        cout << n.read() << " is " << (is_prime.read() ? "prime" : "NOT prime") << endl;

        // Test case 2: Non-prime number
        n = 30;
        wait(1, SC_NS);
        assert(is_prime.read() == false);
        cout << n.read() << " is " << (is_prime.read() ? "prime" : "NOT prime") << endl;

        // Test case 3: Edge case (1)
        n = 1;
        wait(1, SC_NS);
        assert(is_prime.read() == false);
        cout << n.read() << " is " << (is_prime.read() ? "prime" : "NOT prime") << endl;

        // Test case 4: Edge case (0)
        n = 0;
        wait(1, SC_NS);
        assert(is_prime.read() == false);
        cout << n.read() << " is " << (is_prime.read() ? "prime" : "NOT prime") << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
