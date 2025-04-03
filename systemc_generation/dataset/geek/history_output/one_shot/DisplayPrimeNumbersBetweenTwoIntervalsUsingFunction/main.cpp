
#include <systemc.h>

// Module to find prime numbers in a given range
SC_MODULE(PrimeFinder) {
    sc_in<int> l;  // Lower bound of the range
    sc_in<int> r;  // Upper bound of the range
    sc_out<bool> done; // Indicates completion of the task

    // Constructor
    SC_CTOR(PrimeFinder) {
        // Process to find primes
        SC_METHOD(find_primes);
        sensitive << l << r;
        dont_initialize();
    }

    // Method to find prime numbers
    void find_primes() {
        int lower = l.read();
        int upper = r.read();
        bool found = false;

        // Check each number in the range
        for (int i = lower; i <= upper; i++) {
            if (is_prime(i)) {
                std::cout << i << " ";
                found = true;
            }
        }

        if (!found) {
            std::cout << "No prime numbers found in the given range.";
        }

        std::cout << std::endl;
        done.write(true);
    }

    // Helper function to check if a number is prime
    bool is_prime(int n) {
        if (n <= 1)
            return false;
        for (int i = 2; i < n; i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> l; // Lower bound signal
    sc_signal<int> r; // Upper bound signal
    sc_signal<bool> done; // Completion signal

    PrimeFinder prime_finder_inst; // Instance of the PrimeFinder module

    // Constructor
    SC_CTOR(Testbench) : prime_finder_inst("prime_finder") {
        // Connect signals to ports
        prime_finder_inst.l(l);
        prime_finder_inst.r(r);
        prime_finder_inst.done(done);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the range
        l = 10;
        r = 30;
        wait(1, SC_NS); // Wait for the PrimeFinder to process

        // Wait until the task is done
        while (!done.read()) {
            wait(1, SC_NS);
        }

        // Print completion message
        std::cout << "Prime finding completed." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
