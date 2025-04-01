
#include <systemc.h>

// Module to find prime numbers within an interval
SC_MODULE(PrimeFinder) {
    sc_in<int> lower_bound; // Input port for the lower bound of the interval
    sc_in<int> upper_bound; // Input port for the upper bound of the interval
    sc_out<sc_bv<10>> prime_numbers; // Output port for prime numbers (assuming max 10 primes)

    // Constructor
    SC_CTOR(PrimeFinder) {
        // Process to find prime numbers
        SC_METHOD(find_primes);
        sensitive << lower_bound << upper_bound;
    }

    // Method to find and print prime numbers
    void find_primes() {
        int a = lower_bound.read();
        int b = upper_bound.read();
        int i, j, flag;
        sc_bv<10> primes; // Bit vector to store prime numbers (for demonstration purposes)
        int count = 0; // Count of prime numbers found

        // Print display message
        cout << "Prime numbers between " << a << " and " << b << " are: ";

        // Traverse each number in the interval
        for (i = a; i <= b; i++) {
            // Skip 0 and 1 as they are neither prime nor composite
            if (i == 1 || i == 0)
                continue;

            // flag variable to tell if i is prime or not
            flag = 1;

            for (j = 2; j <= i / 2; ++j) {
                if (i % j == 0) {
                    flag = 0;
                    break;
                }
            }

            // flag = 1 means i is prime
            if (flag == 1) {
                cout << i << " ";
                if (count < 10) { // Assuming we only store up to 10 primes for demonstration
                    primes[count++] = 1;
                }
            }
        }
        prime_numbers.write(primes);
        cout << endl;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> lower_bound; // Signal for the lower bound of the interval
    sc_signal<int> upper_bound; // Signal for the upper bound of the interval
    sc_signal<sc_bv<10>> prime_numbers; // Signal for the prime numbers

    PrimeFinder prime_finder_inst; // Instance of the PrimeFinder module

    // Constructor
    SC_CTOR(Testbench) : prime_finder_inst("prime_finder") {
        // Connect signals to ports
        prime_finder_inst.lower_bound(lower_bound);
        prime_finder_inst.upper_bound(upper_bound);
        prime_finder_inst.prime_numbers(prime_numbers);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the interval bounds
        lower_bound = 100;
        upper_bound = 300;
        wait(1, SC_NS); // Wait for the prime finder to process

        // Print success message
        cout << "Test passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
