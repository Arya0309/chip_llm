
#include <systemc.h>

// Module to represent a Complex Number
SC_MODULE(ComplexNumber) {
    sc_in<int> real_in;       // Input for real part
    sc_in<int> imag_in;       // Input for imaginary part
    sc_out<int> real_out;     // Output for real part
    sc_out<int> imag_out;     // Output for imaginary part

    // Constructor
    SC_CTOR(ComplexNumber) {
        // Process to assign input values to output
        SC_METHOD(assign_values);
        sensitive << real_in << imag_in;
    }

    // Method to assign input values to output
    void assign_values() {
        real_out.write(real_in.read());
        imag_out.write(imag_in.read());
    }
};

// Module to add two Complex Numbers
SC_MODULE(ComplexAdder) {
    sc_in<int> real1;          // Real part of first complex number
    sc_in<int> imag1;          // Imaginary part of first complex number
    sc_in<int> real2;          // Real part of second complex number
    sc_in<int> imag2;          // Imaginary part of second complex number
    sc_out<int> real_sum;      // Real part of the sum
    sc_out<int> imag_sum;      // Imaginary part of the sum

    // Constructor
    SC_CTOR(ComplexAdder) {
        // Process to perform addition
        SC_METHOD(add_complex);
        sensitive << real1 << imag1 << real2 << imag2;
    }

    // Method to add two complex numbers
    void add_complex() {
        real_sum.write(real1.read() + real2.read());
        imag_sum.write(imag1.read() + imag2.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> real1, imag1; // Signals for first complex number
    sc_signal<int> real2, imag2; // Signals for second complex number
    sc_signal<int> real_sum, imag_sum; // Signals for the sum of complex numbers

    ComplexNumber c1; // Instance of ComplexNumber module for C1
    ComplexNumber c2; // Instance of ComplexNumber module for C2
    ComplexAdder adder; // Instance of ComplexAdder module

    // Constructor
    SC_CTOR(Testbench) : c1("c1"), c2("c2"), adder("adder") {
        // Connect signals to ports
        c1.real_in(real1);
        c1.imag_in(imag1);
        c1.real_out(real1);
        c1.imag_out(imag1);

        c2.real_in(real2);
        c2.imag_in(imag2);
        c2.real_out(real2);
        c2.imag_out(imag2);

        adder.real1(real1);
        adder.imag1(imag1);
        adder.real2(real2);
        adder.imag2(imag2);
        adder.real_sum(real_sum);
        adder.imag_sum(imag_sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize complex numbers
        real1 = 3; imag1 = 2;
        real2 = 9; imag2 = 5;
        wait(1, SC_NS); // Wait for the adder to process

        // Print the sum of complex numbers
        cout << "Complex number 1 : " << real1.read() << " + i" << imag1.read() << endl;
        cout << "Complex number 2 : " << real2.read() << " + i" << imag2.read() << endl;
        cout << "Sum of complex number : " << real_sum.read() << " + i" << imag_sum.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
