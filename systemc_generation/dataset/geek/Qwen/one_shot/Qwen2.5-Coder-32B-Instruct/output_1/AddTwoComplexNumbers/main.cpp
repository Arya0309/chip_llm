
#include <systemc.h>

// SystemC module to add two complex numbers

SC_MODULE(ComplexAdder) {
    // Input ports for real and imaginary parts of two complex numbers
    sc_in<int> real1;
    sc_in<int> imag1;
    sc_in<int> real2;
    sc_in<int> imag2;
    
    // Output ports for real and imaginary parts of the sum of the complex numbers
    sc_out<int> real_sum;
    sc_out<int> imag_sum;

    // Constructor
    SC_CTOR(ComplexAdder) {
        // Sensitivity list for the process
        SC_METHOD(add_complex_numbers);
        sensitive << real1 << imag1 << real2 << imag2;
    }

    // Method to add two complex numbers
    void add_complex_numbers() {
        // Read the real and imaginary parts from the input ports
        int r1 = real1.read();
        int i1 = imag1.read();
        int r2 = real2.read();
        int i2 = imag2.read();

        // Calculate the sum of the real and imaginary parts
        int real_result = r1 + r2;
        int imag_result = i1 + i2;

        // Write the results to the output ports
        real_sum.write(real_result);
        imag_sum.write(imag_result);
    }
};

// Example usage in a testbench


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
