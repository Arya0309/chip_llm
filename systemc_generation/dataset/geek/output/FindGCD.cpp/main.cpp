
#include <systemc.h>

// User Defined GCD module
SC_MODULE(GCDModule) {
    // Input ports for two numbers
    sc_in<int> num1, num2;
    
    // Output port for the GCD
    sc_out<int> gcd_result;

    // Process to calculate GCD
    void calculateGCD() {
        // Reading inputs
        int a = num1.read();
        int b = num2.read();

        // Initialize result with the smaller of a and b
        int res = (a < b) ? a : b;

        // Finding GCD using a loop
        while (res > 1) {
            if (a % res == 0 && b % res == 0)
                break;
            res--;
        }

        // Writing the GCD result
        gcd_result.write(res);
    }

    // Constructor to register the process
    SC_CTOR(GCDModule) {
        SC_METHOD(calculateGCD);
        sensitive << num1 << num2;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with GCDModule
    sc_signal<int> a, b;
    sc_signal<int> gcd_result;

    // Instance of GCDModule
    GCDModule gcd_module;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing numbers
        a = 12; b = 16;

        // Wait for one delta cycle to let the GCD module process
        wait(1, SC_NS);

        // Print the results
        cout << "GCD of " << a.read() << " and " << b.read() << " is " << gcd_result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : gcd_module("gcd_module") {
        // Connecting signals to the GCD module ports
        gcd_module.num1(a); gcd_module.num2(b);
        gcd_module.gcd_result(gcd_result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
