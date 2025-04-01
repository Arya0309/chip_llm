
#include <systemc.h>

// Module that computes power function
SC_MODULE(Power) {
    sc_in<int> a;          // Input port for the base
    sc_in<int> b;          // Input port for the exponent
    sc_out<int> result;     // Output port for the result

    // Constructor
    SC_CTOR(Power) {
        // Process to compute power
        SC_METHOD(compute_power);
        sensitive << a << b;
    }

    // Method to compute power
    void compute_power() {
        int base = a.read();
        int exp = b.read();
        
        if (exp == 0) {
            result.write(1);
            return;
        }
        
        int answer = base;
        int increment = base;
        
        for(int i = 1; i < exp; i++) {
            for(int j = 1; j < base; j++) {
                answer += increment;
            }
            increment = answer;
        }
        
        result.write(answer);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a;      // Signal for the base
    sc_signal<int> b;      // Signal for the exponent
    sc_signal<int> result; // Signal for the result

    Power power_inst;       // Instance of the Power module

    // Constructor
    SC_CTOR(Testbench) : power_inst("power") {
        // Connect signals to ports
        power_inst.a(a);
        power_inst.b(b);
        power_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Base = 5, Exponent = 3
        a = 5; b = 3;
        wait(1, SC_NS); // Wait for the power module to process
        assert(result.read() == 125);
        cout << "5^3 = " << result.read() << endl;

        // Test case 2: Base = 2, Exponent = 4
        a = 2; b = 4;
        wait(1, SC_NS);
        assert(result.read() == 16);
        cout << "2^4 = " << result.read() << endl;

        // Test case 3: Base = 10, Exponent = 2
        a = 10; b = 2;
        wait(1, SC_NS);
        assert(result.read() == 100);
        cout << "10^2 = " << result.read() << endl;

        // Test case 4: Base = 0, Exponent = 5
        a = 0; b = 5;
        wait(1, SC_NS);
        assert(result.read() == 0);
        cout << "0^5 = " << result.read() << endl;

        // Test case 5: Base = 7, Exponent = 0
        a = 7; b = 0;
        wait(1, SC_NS);
        assert(result.read() == 1);
        cout << "7^0 = " << result.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
