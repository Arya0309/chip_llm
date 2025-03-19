structure_examples_1 = """
#include <systemc.h>

// User Defined Complex module
SC_MODULE(ComplexAdder) {
    // Input ports for real and imaginary parts of two complex numbers
    sc_in<int> real1, imag1; // For C1
    sc_in<int> real2, imag2; // For C2
    
    // Output ports for real and imaginary parts of the sum
    sc_out<int> realSum, imagSum;

    // Process to add two complex numbers
    void addComplexNumbers() {
        // Reading inputs
        int r1 = real1.read();
        int i1 = imag1.read();
        int r2 = real2.read();
        int i2 = imag2.read();

        // Calculating the sum of real and imaginary parts
        int sumReal = r1 + r2;
        int sumImag = i1 + i2;

        // Writing outputs
        realSum.write(sumReal);
        imagSum.write(sumImag);
    }

    // Constructor to register the process
    SC_CTOR(ComplexAdder) {
        SC_METHOD(addComplexNumbers);
        sensitive << real1 << imag1 << real2 << imag2;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with ComplexAdder
    sc_signal<int> real1, imag1; // For C1
    sc_signal<int> real2, imag2; // For C2
    sc_signal<int> realSum, imagSum;

    // Instance of ComplexAdder
    ComplexAdder adder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing Complex numbers
        real1 = 3; imag1 = 2;
        real2 = 9; imag2 = 5;

        // Wait for one delta cycle to let the adder process
        wait(1, SC_NS);

        // Print the results
        cout << "Complex number 1 : " << real1.read() << " + i" << imag1.read() << endl;
        cout << "Complex number 2 : " << real2.read() << " + i" << imag2.read() << endl;
        cout << "Sum of complex number : " << realSum.read() << " + i" << imagSum.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : adder("adder") {
        // Connecting signals to the adder ports
        adder.real1(real1); adder.imag1(imag1);
        adder.real2(real2); adder.imag2(imag2);
        adder.realSum(realSum); adder.imagSum(imagSum);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}"""

structure_examples_2 = """
#include <systemc.h>

// SystemC module to convert octal to decimal
SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_in;  // Input port for octal number
    sc_out<int> decimal_out;  // Output port for decimal number

    void conversion_process() {
        int num = octal_in.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        decimal_out.write(dec_value);
    }

    SC_CTOR(OctalToDecimalConverter) {
        SC_METHOD(conversion_process);
        sensitive << octal_in;
    }
};

// Testbench for OctalToDecimalConverter
SC_MODULE(Testbench) {
    sc_signal<int> octal_signal;
    sc_signal<int> decimal_signal;

    OctalToDecimalConverter converter{"converter"};
    
    SC_CTOR(Testbench) {
        converter.octal_in(octal_signal);
        converter.decimal_out(decimal_signal);

        SC_THREAD(test_process);
    }

    void test_process() {
        octal_signal = 67;  // Input octal number
        wait(1, SC_NS);  // Wait for the conversion process to complete
        cout << "Octal: 67 -> Decimal: " << decimal_signal.read() << endl;
        sc_stop();  // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb{"tb"};
    sc_start();  // Start the simulation
    return 0;
}"""
