
#include <systemc.h>

// Module to compute GCD of two numbers
SC_MODULE(GCD_Computer) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> gcd_result; // Output port for the GCD

    // Constructor
    SC_CTOR(GCD_Computer) {
        // Process to compute GCD
        SC_METHOD(compute_gcd);
        sensitive << a << b;
    }

    // Method to compute the GCD
    void compute_gcd() {
        int x = a.read();
        int y = b.read();
        int res = std::min(x, y);

        while (res > 1) {
            if (x % res == 0 && y % res == 0)
                break;
            res--;
        }
        gcd_result.write(res);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the first number
    sc_signal<int> b; // Signal for the second number
    sc_signal<int> gcd_result; // Signal for the GCD

    GCD_Computer gcd_inst; // Instance of the GCD_Computer module

    // Constructor
    SC_CTOR(Testbench) : gcd_inst("gcd_computer") {
        // Connect signals to ports
        gcd_inst.a(a);
        gcd_inst.b(b);
        gcd_inst.gcd_result(gcd_result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: GCD of 12 and 16
        a = 12; b = 16;
        wait(1, SC_NS); // Wait for the GCD computation
        assert(gcd_result.read() == 4);
        cout << "GCD of 12 and 16 is: " << gcd_result.read() << endl;

        // Additional test case: GCD of 25 and 15
        a = 25; b = 15;
        wait(1, SC_NS);
        assert(gcd_result.read() == 5);
        cout << "GCD of 25 and 15 is: " << gcd_result.read() << endl;

        // Additional test case: GCD of 7 and 3
        a = 7; b = 3;
        wait(1, SC_NS);
        assert(gcd_result.read() == 1);
        cout << "GCD of 7 and 3 is: " << gcd_result.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        // User interaction simulation
        a = 48; b = 18; // Example user input
        wait(1, SC_NS);
        cout << "GCD of 48 and 18 is: " << gcd_result.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
