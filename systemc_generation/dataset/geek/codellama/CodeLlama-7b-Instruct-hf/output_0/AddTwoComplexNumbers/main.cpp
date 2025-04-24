
#include <systemc.h>

// The ComplexAdder class is defined in C++

SC_MODULE(ComplexAdder) {
    sc_in<int> real;     // input port for real part of complex number
    sc_in<int> imaginary; // input port for imaginary part of complex number
    sc_out<int> temp;    // output port for temporary sum

    // constructor for ComplexAdder class
    SC_CTOR(ComplexAdder) : real(real), imaginary(imaginary) { }

    // addComp method for adding two complex numbers
    SC_METHOD(addComp, real, imaginary) {
        // temporary variable for storing sum
        ComplexAdder temp();

        // add real and imaginary parts of complex numbers
        temp.real = real + imaginary;
        temp.imaginary = temp.imaginary + imaginary;

        // return sum
        return temp;
    }
};

// Main() function

SC_MODULE(Main) {
    // sc_in<int> for real and imaginary parts of 2 complex numbers
    sc_in<int> real1, real2;
    sc_in<int> imag1, imag2;

    // sc_out<int> for real and imaginary parts of sum of complex numbers
    sc_out<int> real_sum, imag_sum;

    // ComplexAdder object for C1
    ComplexAdder C1(real1, imag1);

    // ComplexAdder objec for C2
    ComplexAdder C2(real2, imag2);

    // SC_METHOD for adding complex numbers
    SC_METHOD(add) {
        // create temporary variable for sum
        ComplexAdder temp();

        // add real and imaginary parts of complex numbers
        temp.real = real1 + imag1;
        temp.imaginary = temp.imaginary + imag2;

        // return sum
        return temp;
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
