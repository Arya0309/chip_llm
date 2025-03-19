
#include <systemc.h>

// Factorial Calculation Module
SC_MODULE(FactorialCalculator) {
    sc_in<unsigned int> input;    // Input port for the number to calculate factorial
    sc_out<unsigned int> output;  // Output port for the factorial result

    // Process to calculate factorial
    void calculateFactorial() {
        unsigned int n = input.read();
        unsigned int res = 1;
        for (unsigned int i = 2; i <= n; i++) {
            res *= i;
        }
        output.write(res);
    }

    // Constructor to register the process
    SC_CTOR(FactorialCalculator) {
        SC_METHOD(calculateFactorial);
        sensitive << input;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    sc_signal<unsigned int> num;  // Signal for the number input
    sc_signal<unsigned int> fact; // Signal for the factorial output

    // Instance of FactorialCalculator
    FactorialCalculator calc;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        num = 5; // Set the number to 5
        wait(1, SC_NS); // Wait for one delta cycle to let the calculator process

        // Print the results
        cout << "Factorial of " << num.read() << " is " << fact.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calc("calc") {
        // Connecting signals to the calculator ports
        calc.input(num);
        calc.output(fact);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
