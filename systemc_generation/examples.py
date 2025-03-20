structure_examples_1 = """
#include <systemc.h>

// Module that adds two natural numbers
SC_MODULE(Adder) {
    sc_in<unsigned int> a;  // Input port for the first number
    sc_in<unsigned int> b;  // Input port for the second number
    sc_out<unsigned int> sum; // Output port for the sum

    // Constructor
    SC_CTOR(Adder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to add the two numbers
    void add() {
        sum.write(a.read() + b.read());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<unsigned int> a; // Signal for the first number
    sc_signal<unsigned int> b; // Signal for the second number
    sc_signal<unsigned int> sum; // Signal for the sum

    Adder adder_inst; // Instance of the Adder module

    // Constructor
    SC_CTOR(Testbench) : adder_inst("adder") {
        // Connect signals to ports
        adder_inst.a(a);
        adder_inst.b(b);
        adder_inst.sum(sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Adding zero values
        // a = 0; b = 0;
        // wait(1, SC_NS); // Wait for the adder to process
        // assert(sum.read() == 0);
        // cout << sum.read() << endl; 

        // Test case 2: Simple addition
        a = 1; b = 2;
        wait(1, SC_NS);
        assert(sum.read() == 3);
        cout << sum.read() << endl;

        // Test case 3: Larger numbers
        a = 100; b = 200;
        wait(1, SC_NS);
        assert(sum.read() == 300);
        cout << sum.read() << endl;

        // Additional test case: One operand is zero
        a = 0; b = 5;
        wait(1, SC_NS);
        assert(sum.read() == 5);
        cout << sum.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        // User interaction simulation
        a = 3; b = 7; // Example user input
        wait(1, SC_NS);
        cout << "Sum: " << sum.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}"""

structure_examples_2 = """
#include <systemc.h>

// Module to calculate the GCD of two numbers
SC_MODULE(GcdCalculator) {
    // Input ports for two integers
    sc_in<int> a;
    sc_in<int> b;
    // Output port for the result (GCD)
    sc_out<int> gcd_result;

    // Constructor: Register the method to compute GCD
    SC_CTOR(GcdCalculator) {
        SC_METHOD(compute_gcd);
        sensitive << a << b;
    }

    // Method to compute the GCD
    void compute_gcd() {
        int x = a.read();
        int y = b.read();
        int res = (x < y) ? x : y; // Equivalent to min(x, y)
        
        // Loop from min(x, y) down to 1
        while (res > 1) {
            if ((x % res == 0) && (y % res == 0))
                break;
            res--;
        }
        gcd_result.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a_sig;       // Signal for the first input
    sc_signal<int> b_sig;       // Signal for the second input
    sc_signal<int> gcd_sig;     // Signal for the GCD output

    // Instance of the GcdCalculator module
    GcdCalculator gcd_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : gcd_inst("gcd_inst") {
        // Connect signals to module ports
        gcd_inst.a(a_sig);
        gcd_inst.b(b_sig);
        gcd_inst.gcd_result(gcd_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: a = 12, b = 16 (expected GCD = 4)
        a_sig.write(12);
        b_sig.write(16);
        wait(1, SC_NS); // Wait for the process to update
        int result = gcd_sig.read();
        std::cout << "GCD(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 4);

        // End simulation after successful test
        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}"""
