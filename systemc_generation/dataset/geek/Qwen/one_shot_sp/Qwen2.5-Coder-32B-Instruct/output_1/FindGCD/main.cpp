
#include <systemc.h>

// Module to compute GCD of two integers

SC_MODULE(GcdCalculator) {
    sc_in<int> a;          // Input port for the first number
    sc_in<int> b;          // Input port for the second number
    sc_out<int> gcd_result; // Output port for the GCD

    // Constructor
    SC_CTOR(GcdCalculator) {
        // Process to compute GCD
        SC_METHOD(compute_gcd);
        sensitive << a << b;
    }

    // Method to compute the GCD
    void compute_gcd() {
        int res = std::min(a.read(), b.read()); // Find the minimum of a and b

        // Loop to find GCD
        while (res > 1) {
            if (a.read() % res == 0 && b.read() % res == 0)
                break;
            res--;
        }

        gcd_result.write(res); // Write the result to the output port
    }
};

// Example testbench

SC_MODULE(TB) {
    sc_signal<int> a;          // Signal for the first number
    sc_signal<int> b;          // Signal for the second number
    sc_signal<int> gcd_result; // Signal for the GCD result

    GcdCalculator gcd_computer; // Instance of the GcdCalculator module

    SC_CTOR(TB) : gcd_computer("gcd_computer") {
        // Connect the signals to the module's ports
        gcd_computer.a(a);
        gcd_computer.b(b);
        gcd_computer.gcd_result(gcd_result);

        // Process to drive inputs and read output
        SC_THREAD(test);
    }

    void test() {
        a = 12;
        b = 16;
        wait(1, SC_NS); // Wait for one nanosecond to allow the computation
        std::cout << "GCD of " << a.read() << " and " << b.read() << " is: " << gcd_result.read() << std::endl;

        // Additional test cases can be added here
        a = 48;
        b = 18;
        wait(1, SC_NS);
        std::cout << "GCD of " << a.read() << " and " << b.read() << " is: " << gcd_result.read() << std::endl;

        sc_stop(); // Stop simulation
    }
};



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

        // Additional test case: a = 48, b = 18 (expected GCD = 6)
        a_sig.write(48);
        b_sig.write(18);
        wait(1, SC_NS);
        result = gcd_sig.read();
        std::cout << "GCD(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 6);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
