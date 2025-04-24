
#include <systemc.h>

// Module that adds two complex numbers represented as (real, imaginary) pairs

SC_MODULE(ComplexAdder) {
    // Inputs: the real and imaginary parts of the first complex number
    sc_in<int> real1;
    sc_in<int> imag1;

    // Inputs: the real and imaginary parts of the second complex number
    sc_in<int> real2;
    sc_in<int> imag2;

    // Outputs: the real and imaginary parts of the sum of the two complex numbers
    sc_out<int> real_sum;
    sc_out<int> imag_sum;

    // Constructor
    SC_CTOR(ComplexAdder) {
        // Process to read and add the complex numbers
        SC_METHOD(add_complex);
        sensitive << real1 << imag1 << real2 << imag2;
    }

    // Method to add the complex numbers
    void add_complex() {
        // Create temporary Complex objects for the two input numbers
        Complex c1(real1.read(), imag1.read());
        Complex c2(real2.read(), imag2.read());

        // Use the addComp method to compute the sum
        Complex c3 = c1.addComp(c2);

        // Write the real and imaginary parts of the sum to the outputs
        real_sum.write(c3.real);
        imag_sum.write(c3.imaginary);
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> real1;   // Signal for the real part of the first complex number
    sc_signal<int> imag1;   // Signal for the imaginary part of the first complex number
    sc_signal<int> real2;   // Signal for the real part of the second complex number
    sc_signal<int> imag2;   // Signal for the imaginary part of the second complex number
    sc_signal<int> real_sum;  // Signal for the real part of the sum
    sc_signal<int> imag_sum;  // Signal for the imaginary part of the sum

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
        
        // Use assertions to check that the sum is as expected
        assert(real_sum.read() == 12 && "Real sum is incorrect!");
        assert(imag_sum.read() == 7 && "Imaginary sum is incorrect!");

        // Test case: Adding complex numbers (3 - 3i) and (9 + 5i)
        real1 = 3; imag1 = -3;
        real2 = 9; imag2 = 5;
        wait(1, SC_NS); // Wait for the adder to process

        // Use assertions to check that the sum is as expected
        assert(real_sum.read() == 12 && "Real sum is incorrect!");
        assert(imag_sum.read() == 2 && "Imaginary sum is incorrect!");

        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
