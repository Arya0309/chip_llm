
#include <systemc.h>

// Module to add two complex numbers
SC_MODULE(ComplexAdder) {
    // Input ports for the real and imaginary parts of the first complex number
    sc_in<int> real1;
    sc_in<int> imag1;
    // Input ports for the real and imaginary parts of the second complex number
    sc_in<int> real2;
    sc_in<int> imag2;
    // Output ports for the real and imaginary parts of the sum
    sc_out<int> real_sum;
    sc_out<int> imag_sum;

    // Constructor
    SC_CTOR(ComplexAdder) {
        // Process to perform addition
        SC_METHOD(add_complex);
        sensitive << real1 << imag1 << real2 << imag2;
    }

    // Method to add the two complex numbers
    void add_complex() {
        real_sum.write(real1.read() + real2.read());
        imag_sum.write(imag1.read() + imag2.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> real1; // Signal for the real part of the first complex number
    sc_signal<int> imag1; // Signal for the imaginary part of the first complex number
    sc_signal<int> real2; // Signal for the real part of the second complex number
    sc_signal<int> imag2; // Signal for the imaginary part of the second complex number
    sc_signal<int> real_sum; // Signal for the real part of the sum
    sc_signal<int> imag_sum; // Signal for the imaginary part of the sum

    ComplexAdder adder_inst; // Instance of the ComplexAdder module

    // Constructor
    SC_CTOR(Testbench) : adder_inst("adder") {
        // Connect signals to ports
        adder_inst.real1(real1);
        adder_inst.imag1(imag1);
        adder_inst.real2(real2);
        adder_inst.imag2(imag2);
        adder_inst.real_sum(real_sum);
        adder_inst.imag_sum(imag_sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: Adding complex numbers (3 + 2i) and (9 + 5i)
        real1 = 3; imag1 = 2;
        real2 = 9; imag2 = 5;
        wait(1, SC_NS); // Wait for the adder to process
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
