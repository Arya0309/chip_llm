
#include <systemc.h>


SC_MODULE(LcmCalculator) {
    sc_in<int> a;          // Input signal for first number
    sc_in<int> b;          // Input signal for second number
    sc_out<int> lcm_result; // Output signal for LCM

    SC_CTOR(LcmCalculator) {
        SC_METHOD(calculate_lcm);
        sensitive << a << b; // Trigger calculation when 'a' or 'b' changes
    }

    void calculate_lcm() {
        int num1 = a.read();
        int num2 = b.read();
        int max_num = (num1 > num2) ? num1 : num2;
        bool found = false;

        while (!found) {
            if (max_num % num1 == 0 && max_num % num2 == 0) {
                lcm_result.write(max_num);
                found = true;
            }
            ++max_num;
        }
    }
};



SC_MODULE(Testbench) {
    sc_signal<int> a_sig;       // Signal for the first input
    sc_signal<int> b_sig;       // Signal for the second input
    sc_signal<int> lcm_sig;      // Signal for the LCM output

    // Instance of the LcmCalculator module
    LcmCalculator lcm_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : lcm_inst("lcm_inst") {
        // Connect signals to module ports
        lcm_inst.a(a_sig);
        lcm_inst.b(b_sig);
        lcm_inst.lcm_result(lcm_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: a = 15, b = 20 (expected LCM = 60)
        a_sig.write(15);
        b_sig.write(20);
        wait(1, SC_NS); // Wait for the process to update
        int result = lcm_sig.read();
        std::cout << "LCM(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 60);

        // Test case: a = 17, b = 15 (expected LCM = 255)
        a_sig.write(17);
        b_sig.write(15);
        wait(1, SC_NS); // Wait for the process to update
        result = lcm_sig.read();
        std::cout << "LCM(" << a_sig.read() << ", " << b_sig.read() << ") = " << result << std::endl;
        assert(result == 255);

        // End simulation after successful test
        std::cout << "Test passed successfully." << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
