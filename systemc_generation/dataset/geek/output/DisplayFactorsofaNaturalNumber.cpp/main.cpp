
#include <systemc.h>

SC_MODULE(DivisorPrinter) {
    SC_CTOR(DivisorPrinter) {
        SC_THREAD(print_divisors);
    }

    void print_divisors() {
        int n = 100; // Number for which divisors are to be found
        sc_start(1, SC_NS); // Start simulation for a short duration
        cout << "The divisors of " << n << " are: ";
        for (int i = 1; i <= n; ++i) {
            if (n % i == 0) {
                cout << " " << i;
            }
        }
        cout << endl;
        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    DivisorPrinter dp("DivisorPrinter");
    sc_start(); // Start the simulation
    return 0;
}
