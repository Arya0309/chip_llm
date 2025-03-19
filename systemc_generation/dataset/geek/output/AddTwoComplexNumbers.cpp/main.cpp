
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
}
