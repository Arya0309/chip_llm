
#include <systemc.h>

// User Defined PrimeChecker module
SC_MODULE(PrimeChecker) {
    // Input port for the number to be checked
    sc_in<int> number_in;
    
    // Output port for the result
    sc_out<bool> is_prime_out;

    // Process to check if the number is prime
    void checkPrime() {
        int n = number_in.read();
        int cnt = 0;

        // If number is less than/equal to 1,
        // it is not prime
        if (n <= 1) {
            is_prime_out.write(false);
            return;
        }

        // Check for divisors from 1 to n
        for (int i = 1; i <= n; i++) {
            // Check how many number is divisible by n
            if (n % i == 0)
                cnt++;
        }

        // If n is divisible by more than 2 numbers
        // then it is not prime
        if (cnt > 2)
            is_prime_out.write(false);
        else
            is_prime_out.write(true);
    }

    // Constructor to register the process
    SC_CTOR(PrimeChecker) {
        SC_METHOD(checkPrime);
        sensitive << number_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with PrimeChecker
    sc_signal<int> number;
    sc_signal<bool> is_prime;

    // Instance of PrimeChecker
    PrimeChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the number to be checked
        number = 29;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the result
        if (is_prime.read())
            cout << number.read() << " is prime" << endl;
        else
            cout << number.read() << " is NOT prime" << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.number_in(number);
        checker.is_prime_out(is_prime);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
