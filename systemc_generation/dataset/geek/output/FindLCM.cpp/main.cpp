
#include <systemc.h>

// Module to calculate LCM of two numbers
SC_MODULE(LCMCalculator) {
    // Input ports
    sc_in<int> num1;
    sc_in<int> num2;

    // Output port
    sc_out<int> lcm_result;

    // Internal variables
    int a, b, max_num, flag;

    // Process to calculate LCM
    void calculateLCM() {
        // Initialize internal variables with input values
        a = num1.read();
        b = num2.read();
        max_num = (a > b) ? a : b;
        flag = 1;

        // Calculate LCM using a while loop
        while (flag) {
            if (max_num % a == 0 && max_num % b == 0) {
                lcm_result.write(max_num);
                flag = 0; // Exit the loop
            } else {
                ++max_num;
            }
            wait(1, SC_NS); // Wait for one delta cycle
        }
    }

    // Constructor to register the process
    SC_CTOR(LCMCalculator) {
        SC_THREAD(calculateLCM);
        sensitive << num1 << num2; // Sensitivity list
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with LCMCalculator
    sc_signal<int> num1, num2;
    sc_signal<int> lcm_result;

    // Instance of LCMCalculator
    LCMCalculator lcm_calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Drive inputs
        num1 = 15;
        num2 = 20;

        // Wait for the LCM calculation to complete
        wait(10, SC_NS); // Arbitrary wait time to ensure calculation completes

        // Print the result
        cout << "LCM of " << num1.read() << " and " << num2.read() << " is " << lcm_result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : lcm_calculator("lcm_calculator") {
        // Connecting signals to the calculator ports
        lcm_calculator.num1(num1);
        lcm_calculator.num2(num2);
        lcm_calculator.lcm_result(lcm_result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
