
#include <systemc.h>


SC_MODULE(Divider) {
    sc_in<int> Dividend;  // Input port for the dividend
    sc_in<int> Divisor;   // Input port for the divisor
    sc_out<int> Quotient; // Output port for the quotient
    sc_out<int> Remainder; // Output port for the remainder
    sc_out<bool> Error; // Output port for error flag

    // Constructor
    SC_CTOR(Divider) {
        // Process to handle division
        SC_METHOD(divide);
        sensitive << Dividend << Divisor;
    }

    // Method to perform division
    void divide() {
        if (Divisor.read() == 0) {
            Error.write(true);
            Quotient.write(0);
            Remainder.write(0);
        } else {
            Error.write(false);
            Quotient.write(Dividend.read() / Divisor.read());
            Remainder.write(Dividend.read() % Divisor.read());
        }
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> Dividend;   // Signal for the dividend
    sc_signal<int> Divisor;    // Signal for the divisor
    sc_signal<int> Quotient;   // Signal for the quotient
    sc_signal<int> Remainder;  // Signal for the remainder
    sc_signal<bool> Error;     // Signal to indicate division by zero error

    Divider divider_inst;       // Instance of the Divider module

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
        Dividend = 10;
        Divisor = 0;
        wait(1, SC_NS);
        assert(Error.read() == true && "Division by zero case");
        cout << "Test Case 1: Division by zero - Error: " << Error.read() << endl;

        // Test case 2: Normal division
        Dividend = 20;
        Divisor = 3;
        wait(1, SC_NS);
        assert(Error.read() == false);
        assert(Quotient.read() == 6);
        assert(Remainder.read() == 2);
        cout << "Test Case 2: Quotient: " << Quotient.read() << ", Remainder: " << Remainder.read() << endl;

        // Test case 3: Exact division
        Dividend = 15;
        Divisor = 5;
        wait(1, SC_NS);
        assert(Error.read() == false);
        assert(Quotient.read() == 3);
        assert(Remainder.read() == 0);
        cout << "Test Case 3: Quotient: " << Quotient.read() << ", Remainder: " << Remainder.read() << endl;

        // Test case 4: Negative dividend
        Dividend = -20;
        Divisor = 3;
        wait(1, SC_NS);
        assert(Error.read() == false);
        assert(Quotient.read() == -6);
        assert(Remainder.read() == -2);
        cout << "Test Case 4: Quotient: " << Quotient.read() << ", Remainder: " << Remainder.read() << endl;

        // Test case 5: Negative divisor
        Dividend = 20;
        Divisor = -3;
        wait(1, SC_NS);
        assert(Error.read() == false);
        assert(Quotient.read() == -6);
        assert(Remainder.read() == 2);
        cout << "Test Case 5: Quotient: " << Quotient.read() << ", Remainder: " << Remainder.read() << endl;

        // Test case 6: Both negative
        Dividend = -20;
        Divisor = -3;
        wait(1, SC_NS);
        assert(Error.read() == false);
        assert(Quotient.read() == 6);
        assert(Remainder.read() == -2);
        cout << "Test Case 6: Quotient: " << Quotient.read() << ", Remainder: " << Remainder.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
