
#include <systemc.h>

// Module for computing quotient and remainder
SC_MODULE(Divider) {
    sc_in<int> Dividend;       // Input port for Dividend
    sc_in<int> Divisor;        // Input port for Divisor
    sc_out<int> Quotient;      // Output port for Quotient
    sc_out<int> Remainder;     // Output port for Remainder
    sc_out<bool> Error;         // Output port for Error flag

    // Constructor
    SC_CTOR(Divider) {
        // Process to perform division
        SC_METHOD(divide);
        sensitive << Dividend << Divisor;
    }

    // Method to perform division and compute quotient and remainder
    void divide() {
        if (Divisor.read() == 0) {
            Error.write(true);
            Quotient.write(0); // Default value when error occurs
            Remainder.write(0);
        } else {
            Error.write(false);
            Quotient.write(Dividend.read() / Divisor.read());
            Remainder.write(Dividend.read() % Divisor.read());
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> Dividend;    // Signal for Dividend
    sc_signal<int> Divisor;     // Signal for Divisor
    sc_signal<int> Quotient;    // Signal for Quotient
    sc_signal<int> Remainder;   // Signal for Remainder
    sc_signal<bool> Error;      // Signal for Error flag

    Divider divider_inst;        // Instance of the Divider module

    // Constructor
    SC_CTOR(Testbench) : divider_inst("divider") {
        // Connect signals to ports
        divider_inst.Dividend(Dividend);
        divider_inst.Divisor(Divisor);
        divider_inst.Quotient(Quotient);
        divider_inst.Remainder(Remainder);
        divider_inst.Error(Error);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Division by zero
        Dividend = 10; Divisor = 0;
        wait(1, SC_NS);
        assert(Error.read() == true);
        cout << "Test case 1: Division by zero" << endl;

        // Test case 2: Simple division
        Dividend = 10; Divisor = 3;
        wait(1, SC_NS);
        assert(Quotient.read() == 3 && Remainder.read() == 1);
        cout << "Test case 2: Quotient = " << Quotient.read() << ", Remainder = " << Remainder.read() << endl;

        // Test case 3: Exact division
        Dividend = 20; Divisor = 5;
        wait(1, SC_NS);
        assert(Quotient.read() == 4 && Remainder.read() == 0);
        cout << "Test case 3: Quotient = " << Quotient.read() << ", Remainder = " << Remainder.read() << endl;

        // User interaction simulation
        Dividend = 3; Divisor = 7;
        wait(1, SC_NS);
        cout << "User input: Quotient = " << Quotient.read() << ", Remainder = " << Remainder.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
