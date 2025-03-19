
#include <systemc.h>

// PowerCalculator module to compute power of two numbers
SC_MODULE(PowerCalculator) {
    // Input ports for base and exponent
    sc_in<double> base;
    sc_in<double> exponent;
    
    // Output port for the result
    sc_out<double> result;

    // Process to compute power
    void computePower() {
        // Reading inputs
        double b = base.read();
        double e = exponent.read();

        // Calculating the power
        double res = pow(b, e);

        // Writing output
        result.write(res);
    }

    // Constructor to register the process
    SC_CTOR(PowerCalculator) {
        SC_METHOD(computePower);
        sensitive << base << exponent;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with PowerCalculator
    sc_signal<double> base;
    sc_signal<double> exponent;
    sc_signal<double> result;

    // Instance of PowerCalculator
    PowerCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing base and exponent
        base = 6.1;
        exponent = 4.8;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Base: " << base.read() << ", Exponent: " << exponent.read() << endl;
        cout << "Result: " << result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.base(base);
        calculator.exponent(exponent);
        calculator.result(result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
