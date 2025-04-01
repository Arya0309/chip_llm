
#include <systemc.h>

// Module to find and print prime numbers in a given interval
SC_MODULE(PrimeFinder) {
    sc_in<int> a;  // Input port for the lower bound of the interval
    sc_in<int> b;  // Input port for the upper bound of the interval

    // Constructor
    SC_CTOR(PrimeFinder) {
        // Process to find and print prime numbers
        SC_METHOD(find_primes);
        sensitive << a << b;
    }

    // Method to find and print prime numbers
    void find_primes() {
        int lower_bound = a.read();
        int upper_bound = b.read();
        
        // Print display message
        cout << "Prime numbers between " << lower_bound << " and " << upper_bound << " are: ";

        // Traverse each number in the interval
        for (int i = lower_bound; i <= upper_bound; i++) {
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
            if (flag)
                cout << i << " ";
        }
        cout << endl;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the lower bound of the interval
    sc_signal<int> b; // Signal for the upper bound of the interval

    PrimeFinder prime_finder_inst; // Instance of the PrimeFinder module

    // Constructor
    SC_CTOR(Testbench) : prime_finder_inst("prime_finder") {
        // Connect signals to ports
        prime_finder_inst.a(a);
        prime_finder_inst.b(b);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Given interval [100, 300]
        a = 100; b = 300;
        wait(1, SC_NS); // Wait for the prime_finder to process

        // Additional test case: Another interval [50, 100]
        a = 50; b = 100;
        wait(1, SC_NS);

        // Print success message
        cout << "All tests completed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
