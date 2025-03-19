
#include <systemc.h>

// Module to check if a number can be expressed as the sum of two prime numbers
SC_MODULE(PrimeChecker) {
    // Input port for the number to check
    sc_in<int> number_in;
    
    // Output port for the result ("Yes" or "No")
    sc_out<bool> result_out;

    // Method to check if a number is prime
    bool isPrime(int n) {
        if (n <= 1)
            return false;

        for (int i = 2; i <= sqrt(n); i++) {
            if (n % i == 0)
                return false;
        }

        return true;
    }

    // Method to check if the number can be expressed as the sum of two primes
    void checkPrimeSum() {
        int N = number_in.read();
        bool result = (isPrime(N) && isPrime(N - 2));
        result_out.write(result);
    }

    // Constructor to register the process
    SC_CTOR(PrimeChecker) {
        SC_METHOD(checkPrimeSum);
        sensitive << number_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with PrimeChecker
    sc_signal<int> number;
    sc_signal<bool> result;

    // Instance of PrimeChecker
    PrimeChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the number to check
        number = 13;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the result
        cout << (result.read() ? "Yes" : "No") << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.number_in(number);
        checker.result_out(result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
