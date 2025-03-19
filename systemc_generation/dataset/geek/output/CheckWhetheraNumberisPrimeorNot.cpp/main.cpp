
#include <systemc.h>

SC_MODULE(PrimeChecker) {
    sc_in<int> n;  // Input port for the number to be checked
    sc_out<bool> is_prime;  // Output port indicating if the number is prime

    void check_prime() {
        int cnt = 0;

        // Check if the number is less than or equal to 1
        if (n.read() <= 1) {
            is_prime.write(false);
            return;
        }

        // Loop to count divisors
        for (int i = 1; i <= n.read(); i++) {
            if (n.read() % i == 0) {
                cnt++;
            }
        }

        // Determine if the number is prime
        if (cnt > 2) {
            is_prime.write(false);
        } else {
            is_prime.write(true);
        }
    }

    SC_CTOR(PrimeChecker) {
        SC_METHOD(check_prime);
        sensitive << n;
    }
};

int sc_main(int argc, char* argv[]) {
    PrimeChecker checker("checker");
    sc_signal<int> num;
    sc_signal<bool> result;

    checker.n(num);
    checker.is_prime(result);

    // Test with the number 29
    num.write(29);
    sc_start();

    // Print the result
    if (result.read()) {
        cout << num.read() << " is prime" << endl;
    } else {
        cout << num.read() << " is NOT prime" << endl;
    }

    return 0;
}
