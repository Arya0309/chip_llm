scss
#include <systemc.h>

// Module that adds two complex numbers

SC_MODULE(ComplexAdder) 
{
    // Input signals for the real and imaginary
    // parts of the two complex numbers
    sc_in<int> real1;
    sc_in<int> imag1;
    sc_in<int> real2;
    sc_in<int> imag2;

    // Output signal for the sum of the two complex numbers
    sc_out<int> real_sum;
    sc_out<int> imag_sum;

    // Constructor
    SC_CTOR(ComplexAdder) 
    {
        // Process for adding the two complex numbers
        SC_METHOD(add);
        sensitive << real1 << imag1 << real2 << imag2;
    }

    // Method for adding the two complex numbers
    void add() 
    {
        // Create temporary variables to store the real
        // and imaginary parts of the two complex numbers
        int temp_real1 = real1.read();
        int temp_imag1 = imag1.read();
        int temp_real2 = real2.read();
        int temp_imag2 = imag2.read();

        // Calculate the sum of the real and imaginary parts
        int sum_real = temp_real1 + temp_real2;
        int sum_imag = temp_imag1 + temp_imag2;

        // Assign the sum to the output signal
        real_sum.write(sum_real);
        imag_sum.write(sum_imag);
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
