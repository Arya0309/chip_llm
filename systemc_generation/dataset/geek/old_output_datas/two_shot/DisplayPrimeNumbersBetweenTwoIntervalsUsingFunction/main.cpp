
#include <systemc.h>

// Module to determine if a number is prime
SC_MODULE(PrimeChecker) {
    sc_in<int> num_in;          // Input port for the number to check
    sc_out<bool> is_prime_out;  // Output port for the result (true if prime)

    // Constructor
    SC_CTOR(PrimeChecker) {
        SC_METHOD(check_prime);
        sensitive << num_in;
    }

    // Method to check if the number is prime
    void check_prime() {
        int n = num_in.read();
        if (n <= 1) {
            is_prime_out.write(false);
            return;
        }
        for (int i = 2; i < n; ++i) {
            if (n % i == 0) {
                is_prime_out.write(false);
                return;
            }
        }
        is_prime_out.write(true);
    }
};

// Module to find and print prime numbers in a given range
SC_MODULE(PrimeFinder) {
    sc_in<int> lower_bound;     // Input port for the lower bound of the range
    sc_in<int> upper_bound;     // Input port for the upper bound of the range

    // Internal signal for communication with PrimeChecker
    sc_signal<int> num_to_check;
    sc_signal<bool> prime_result;

    PrimeChecker checker; // Instance of PrimeChecker module

    // Constructor
    SC_CTOR(PrimeFinder)
    : checker("checker") {
        // Connect internal signals to PrimeChecker ports
        checker.num_in(num_to_check);
        checker.is_prime_out(prime_result);

        // Process to find and print prime numbers
        SC_THREAD(find_primes);
    }

    // Thread to find and print prime numbers
    void find_primes() {
        bool found = false;
        for (int i = lower_bound.read(); i <= upper_bound.read(); ++i) {
            num_to_check.write(i);
            wait(1, SC_NS); // Wait for PrimeChecker to process
            if (prime_result.read()) {
                std::cout << i << " ";
                found = true;
            }
        }
        if (!found) {
            std::cout << "No prime numbers found in the given range.";
        }
        std::cout << std::endl;
        sc_stop(); // Stop the simulation after processing
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> lower_bound; // Signal for the lower bound of the range
    sc_signal<int> upper_bound; // Signal for the upper bound of the range

    PrimeFinder finder; // Instance of PrimeFinder module

    // Constructor
    SC_CTOR(Testbench)
    : finder("finder") {
        // Connect signals to PrimeFinder ports
        finder.lower_bound(lower_bound);
        finder.upper_bound(upper_bound);

        // Process to set the range and start the simulation
        SC_THREAD(set_range);
    }

    // Thread to set the range
    void set_range() {
        lower_bound = 10;
        upper_bound = 30;
        wait(1, SC_NS); // Allow some time for PrimeFinder to process
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
