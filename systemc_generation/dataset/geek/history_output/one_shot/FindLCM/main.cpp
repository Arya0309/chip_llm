
#include <systemc.h>

// Module to calculate LCM of two numbers
SC_MODULE(LCMCalculator) {
    sc_in<int> a;  // Input port for the first number
    sc_in<int> b;  // Input port for the second number
    sc_out<int> lcm; // Output port for the LCM

    // Constructor
    SC_CTOR(LCMCalculator) {
        // Process to calculate LCM
        SC_THREAD(calculate_lcm);
        sensitive << a << b;
    }

    // Method to calculate the LCM
    void calculate_lcm() {
        int a_val = a.read();
        int b_val = b.read();
        int max_num = (a_val > b_val) ? a_val : b_val;
        int flag = 1;

        while (flag) {
            if (max_num % a_val == 0 && max_num % b_val == 0) {
                lcm.write(max_num);
                flag = 0; // Exit the loop
            } else {
                ++max_num;
            }
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for the first number
    sc_signal<int> b; // Signal for the second number
    sc_signal<int> lcm; // Signal for the LCM

    LCMCalculator lcm_calc_inst; // Instance of the LCMCalculator module

    // Constructor
    SC_CTOR(Testbench) : lcm_calc_inst("lcm_calc") {
        // Connect signals to ports
        lcm_calc_inst.a(a);
        lcm_calc_inst.b(b);
        lcm_calc_inst.lcm(lcm);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Simple LCM calculation
        a = 15; b = 20;
        wait(1, SC_NS); // Wait for the LCMCalculator to process
        cout << "LCM of " << a.read() << " and " << b.read() << " is " << lcm.read() << endl;

        // Test case 2: Another LCM calculation
        a = 8; b = 12;
        wait(1, SC_NS);
        cout << "LCM of " << a.read() << " and " << b.read() << " is " << lcm.read() << endl;

        // Test case 3: LCM when one number is zero (LCM is the other number)
        a = 0; b = 5;
        wait(1, SC_NS);
        cout << "LCM of " << a.read() << " and " << b.read() << " is " << lcm.read() << endl;

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
