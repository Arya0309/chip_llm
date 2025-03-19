
#include <systemc.h>

// Module to compute power using repeated addition
SC_MODULE(PowerModule) {
    // Input ports for base and exponent
    sc_in<int> base;
    sc_in<int> exponent;
    
    // Output port for result
    sc_out<int> result;

    // Process to compute power
    void computePower() {
        // Reading inputs
        int a = base.read();
        int b = exponent.read();
        
        // Base case: a^0 = 1
        if (b == 0) {
            result.write(1);
            return;
        }
        
        // Initialize variables
        int answer = a;
        int increment = a;
        
        // Outer loop
        for(int i = 1; i < b; i++) {
            int temp_answer = answer;
            // Inner loop: repeated addition
            for(int j = 1; j < a; j++) {
                temp_answer += increment;
            }
            // Update increment
            increment = temp_answer;
            answer = temp_answer;
        }
        
        // Write result
        result.write(answer);
    }

    // Constructor to register the process
    SC_CTOR(PowerModule) {
        SC_METHOD(computePower);
        sensitive << base << exponent;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with PowerModule
    sc_signal<int> base, exponent, result;

    // Instance of PowerModule
    PowerModule powerModule;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing base and exponent
        base = 5; exponent = 3;

        // Wait for one delta cycle to let the powerModule process
        wait(1, SC_NS);

        // Print the results
        cout << "Result of " << base.read() << "^" << exponent.read() << " is " << result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : powerModule("powerModule") {
        // Connecting signals to the powerModule ports
        powerModule.base(base);
        powerModule.exponent(exponent);
        powerModule.result(result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
