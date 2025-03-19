
#include <systemc.h>

// Module to find prime numbers within a specified interval
SC_MODULE(PrimeFinder) {
    // Input ports for the interval bounds
    sc_in<int> lower_bound;
    sc_in<int> upper_bound;

    // Output port to print prime numbers
    sc_out<sc_string> primes;

    // Process to find and print prime numbers
    void findPrimes() {
        int a = lower_bound.read();
        int b = upper_bound.read();
        sc_string result = "Prime numbers between " + std::to_string(a) + " and " + std::to_string(b) + " are: ";

        // Traverse each number in the interval
        for (int i = a; i <= b; i++) {
            // Skip 0 and 1 as they are neither prime nor composite
            if (i == 1 || i == 0)
                continue;

            // flag variable to tell if i is prime or not
            bool flag = true;

            for (int j = 2; j <= i / 2; ++j) {
                if (i % j == 0) {
                    flag = false;
                    break;
                }
            }

            // flag = true means i is prime
            if (flag) {
                result += std::to_string(i) + " ";
            }
        }

        // Write the result to the output port
        primes.write(result);
    }

    // Constructor to register the process
    SC_CTOR(PrimeFinder) {
        SC_METHOD(findPrimes);
        sensitive << lower_bound << upper_bound;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with PrimeFinder
    sc_signal<int> lower_bound, upper_bound;
    sc_signal<sc_string> primes;

    // Instance of PrimeFinder
    PrimeFinder prime_finder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize interval bounds
        lower_bound = 100;
        upper_bound = 300;

        // Wait for one delta cycle to let the prime_finder process
        wait(1, SC_NS);

        // Print the results
        cout << primes.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : prime_finder("prime_finder") {
        // Connecting signals to the prime_finder ports
        prime_finder.lower_bound(lower_bound);
        prime_finder.upper_bound(upper_bound);
        prime_finder.primes(primes);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
