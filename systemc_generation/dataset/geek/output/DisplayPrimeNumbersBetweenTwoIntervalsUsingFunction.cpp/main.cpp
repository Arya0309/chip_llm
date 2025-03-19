
#include <systemc.h>

// Module to find prime numbers in a given range
SC_MODULE(PrimeFinder) {
    // Input ports for the range
    sc_in<int> left_bound;
    sc_in<int> right_bound;

    // Output port for the result (prime numbers)
    sc_out<sc_bv<10>> prime_output; // Assuming maximum 10 primes can be found in the range

    // Internal signal to indicate completion of the operation
    sc_out<bool> done;

    // Process to find prime numbers
    void find_primes() {
        int l = left_bound.read();
        int r = right_bound.read();
        sc_bv<10> primes;
        int index = 0;
        bool found = false;

        for (int i = l; i <= r; i++) {
            if (is_prime(i)) {
                primes.range(index * 4 + 3, index * 4) = i; // Store each prime number in 4 bits
                index++;
                found = true;
            }
        }

        prime_output.write(primes);
        done.write(found);
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

    // Constructor to register the process
    SC_CTOR(PrimeFinder) {
        SC_METHOD(find_primes);
        sensitive << left_bound << right_bound;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with PrimeFinder
    sc_signal<int> left_bound, right_bound;
    sc_signal<sc_bv<10>> prime_output;
    sc_signal<bool> done;

    // Instance of PrimeFinder
    PrimeFinder prime_finder;

    // Process to drive inputs and print outputs
    void drive_and_print() {
        // Initialize the range
        left_bound = 10;
        right_bound = 30;

        // Wait for the done signal
        wait(done.posedge_event());

        // Print the results
        sc_bv<10> primes = prime_output.read();
        bool found = done.read();

        if (found) {
            cout << "Prime numbers in the range [" << left_bound.read() << ", " << right_bound.read() << "] are: ";
            for (int i = 0; i < 10; i++) {
                int prime = primes.range(i * 4 + 3, i * 4).to_int();
                if (prime != 0) { // Check if the slot is not empty
                    cout << prime << " ";
                }
            }
            cout << endl;
        } else {
            cout << "No prime numbers found in the given range." << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : prime_finder("prime_finder") {
        // Connecting signals to the prime_finder ports
        prime_finder.left_bound(left_bound);
        prime_finder.right_bound(right_bound);
        prime_finder.prime_output(prime_output);
        prime_finder.done(done);

        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
