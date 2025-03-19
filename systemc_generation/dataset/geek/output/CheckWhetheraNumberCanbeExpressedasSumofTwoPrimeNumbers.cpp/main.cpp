
#include <systemc.h>

SC_MODULE(PrimeChecker) {
    sc_in<int> input_n;          // Input port for the number to check
    sc_out<bool> output_possible; // Output port indicating possibility

    SC_CTOR(PrimeChecker) {
        SC_METHOD(check_prime);
        sensitive << input_n;
    }

    // Method to check if a number is prime
    bool isPrime(int n) {
        if (n <= 1)
            return false;

        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0)
                return false;
        }

        return true;
    }

    // Method to check if a prime number can be expressed as sum of two primes
    void check_prime() {
        int N = input_n.read();
        bool possible = (isPrime(N) && isPrime(N - 2));
        output_possible.write(possible);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> n;          // Signal for input number
    sc_signal<bool> possible;  // Signal for output possibility

    PrimeChecker checker("checker");
    checker.input_n(n);
    checker.output_possible(possible);

    // Initialize input
    n.write(13);

    // Start simulation
    sc_start();

    // Print result
    if (possible.read())
        cout << "Yes";
    else
        cout << "No";

    return 0;
}
