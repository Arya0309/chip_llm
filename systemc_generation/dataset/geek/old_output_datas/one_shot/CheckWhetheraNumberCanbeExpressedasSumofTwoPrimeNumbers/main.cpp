
#include <systemc.h>

// Module to check if a number is prime
SC_MODULE(PrimeChecker) {
    sc_in<int> number;  // Input port for the number to check
    sc_out<bool> is_prime; // Output port for the result

    // Constructor
    SC_CTOR(PrimeChecker) {
        // Process to check primality
        SC_METHOD(check_prime);
        sensitive << number;
    }

    // Method to check if the number is prime
    void check_prime() {
        int n = number.read();
        if (n <= 1) {
            is_prime.write(false);
            return;
        }
        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0) {
                is_prime.write(false);
                return;
            }
        }
        is_prime.write(true);
    }
};

// Module to check if a prime number can be expressed as sum of two primes
SC_MODULE(SumOfPrimesChecker) {
    sc_in<int> number;  // Input port for the number to check
    sc_out<bool> result; // Output port for the result

    PrimeChecker prime_checker1; // Prime checker for the number itself
    PrimeChecker prime_checker2; // Prime checker for the number minus 2

    // Signals to connect prime checkers
    sc_signal<bool> prime1_result;
    sc_signal<bool> prime2_result;

    // Constructor
    SC_CTOR(SumOfPrimesChecker) :
        prime_checker1("prime_checker1"),
        prime_checker2("prime_checker2")
    {
        // Connect signals to ports
        prime_checker1.number(number);
        prime_checker1.is_prime(prime1_result);

        prime_checker2.number(number - 2);
        prime_checker2.is_prime(prime2_result);

        // Process to determine if the number can be expressed as sum of two primes
        SC_METHOD(check_sum_of_primes);
        sensitive << prime1_result << prime2_result;
    }

    // Method to check if the number can be expressed as sum of two primes
    void check_sum_of_primes() {
        result.write(prime1_result.read() && prime2_result.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number; // Signal for the number to check
    sc_signal<bool> result; // Signal for the result

    SumOfPrimesChecker sum_of_primes_checker_inst; // Instance of the SumOfPrimesChecker module

    // Constructor
    SC_CTOR(Testbench) : sum_of_primes_checker_inst("sum_of_primes_checker") {
        // Connect signals to ports
        sum_of_primes_checker_inst.number(number);
        sum_of_primes_checker_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Number is 13
        number = 13;
        wait(1, SC_NS); // Wait for the checker to process
        if (result.read())
            cout << "Yes" << endl;
        else
            cout << "No" << endl;

        // Additional test case: Number is 17
        number = 17;
        wait(1, SC_NS);
        if (result.read())
            cout << "Yes" << endl;
        else
            cout << "No" << endl;

        // Additional test case: Number is 20
        number = 20;
        wait(1, SC_NS);
        if (result.read())
            cout << "Yes" << endl;
        else
            cout << "No" << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
