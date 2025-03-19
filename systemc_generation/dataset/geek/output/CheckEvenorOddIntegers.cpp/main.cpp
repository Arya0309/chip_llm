
#include <systemc.h>

SC_MODULE(EvenOddChecker) {
    sc_in<int> n;          // Input port for the integer n
    sc_out<bool> result;   // Output port: true for even, false for odd

    SC_CTOR(EvenOddChecker) {
        SC_METHOD(check_even_odd);
        sensitive << n;     // Sensitivity list: react to changes in n
    }

    void check_even_odd() {
        if (n.read() % 2 == 0) {
            result.write(true);  // Write true to indicate even
        } else {
            result.write(false); // Write false to indicate odd
        }
    }
};

int sc_main(int argc, char* argv[]) {
    EvenOddChecker checker("checker");
    sc_signal<int> n;          // Signal to hold the input value
    sc_signal<bool> result;    // Signal to hold the result

    checker.n(n);
    checker.result(result);

    n.write(11);               // Set the input value to 11
    sc_start();                // Start the simulation

    if (result.read()) {
        cout << "Even" << endl;
    } else {
        cout << "Odd" << endl;
    }

    return 0;
}
