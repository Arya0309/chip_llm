
#include <systemc.h>

// User-defined module to compute quotient and remainder
SC_MODULE(Divider) {
    // Input ports
    sc_in<int> dividend;
    sc_in<int> divisor;

    // Output ports
    sc_out<int> quotient;
    sc_out<int> remainder;
    sc_out<bool> error; // True if there's an error (divisor is zero)

    // Process to compute quotient and remainder
    void compute() {
        int divd = dividend.read();
        int divs = divisor.read();
        
        if (divs == 0) {
            // Error: divisor is zero
            error.write(true);
            quotient.write(0);
            remainder.write(0);
        } else {
            // No error, perform division
            error.write(false);
            quotient.write(divd / divs);
            remainder.write(divd % divs);
        }
    }

    // Constructor to register the process
    SC_CTOR(Divider) {
        SC_METHOD(compute);
        sensitive << dividend << divisor;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with Divider
    sc_signal<int> dividend;
    sc_signal<int> divisor;
    sc_signal<int> quotient;
    sc_signal<int> remainder;
    sc_signal<bool> error;

    // Instance of Divider
    Divider divider;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Test case 1: Normal division
        dividend = 10;
        divisor = 3;
        wait(1, SC_NS);
        printResults();

        // Test case 2: Division by zero
        divisor = 0;
        wait(1, SC_NS);
        printResults();

        // Test case 3: Another normal division
        dividend = 20;
        divisor = 4;
        wait(1, SC_NS);
        printResults();
    }

    // Helper function to print results
    void printResults() {
        if (error.read()) {
            cout << "Error: Divisor cannot be zero." << endl;
        } else {
            cout << "The Quotient = " << quotient.read() << endl;
            cout << "The Remainder = " << remainder.read() << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : divider("divider") {
        // Connecting signals to the divider ports
        divider.dividend(dividend);
        divider.divisor(divisor);
        divider.quotient(quotient);
        divider.remainder(remainder);
        divider.error(error);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
