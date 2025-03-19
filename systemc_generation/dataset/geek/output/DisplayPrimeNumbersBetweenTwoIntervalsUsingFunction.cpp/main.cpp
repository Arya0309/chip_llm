
#include <systemc.h>

SC_MODULE(PrimeFinder) {
    sc_in<int> l;
    sc_in<int> r;
    sc_out<sc_bv<32>> prime_numbers;
    sc_out<bool> no_primes_found;

    SC_CTOR(PrimeFinder) {
        SC_METHOD(findPrimes);
        sensitive << l << r;
    }

    bool isPrime(int n) {
        if (n <= 1)
            return false;
        for (int i = 2; i < n; i++) {
            if (n % i == 0)
                return false;
        }
        return true;
    }

    void findPrimes() {
        int lower = l.read();
        int upper = r.read();
        sc_bv<32> primes = 0;
        bool found = false;
        int index = 0;

        for (int i = lower; i <= upper; i++) {
            if (isPrime(i)) {
                primes.range((index + 1) * 8 - 1, index * 8) = i;
                index++;
                found = true;
            }
        }

        prime_numbers.write(primes);
        no_primes_found.write(!found);
    }
};

int sc_main(int argc, char* argv[]) {
    sc_signal<int> l;
    sc_signal<int> r;
    sc_signal<sc_bv<32>> prime_numbers;
    sc_signal<bool> no_primes_found;

    PrimeFinder prime_finder("prime_finder");
    prime_finder.l(l);
    prime_finder.r(r);
    prime_finder.prime_numbers(prime_numbers);
    prime_finder.no_primes_found(no_primes_found);

    l.write(10);
    r.write(30);

    sc_start();

    if (no_primes_found.read()) {
        cout << "No prime numbers found in the given range." << endl;
    } else {
        sc_bv<32> primes = prime_numbers.read();
        cout << "Prime numbers in the given range: ";
        for (int i = 0; i < 32; i += 8) {
            int prime = primes.range(i + 7, i).to_int();
            if (prime != 0) {
                cout << prime << " ";
            }
        }
        cout << endl;
    }

    return 0;
}
