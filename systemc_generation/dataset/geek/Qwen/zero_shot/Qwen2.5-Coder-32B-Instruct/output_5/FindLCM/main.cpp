
#include <systemc.h>


SC_MODULE(LcmCalculator) {
    sc_in<int> a;
    sc_in<int> b;
    sc_out<int> lcm_result;

    SC_CTOR(LcmCalculator) {
        SC_METHOD(calculate_lcm);
        sensitive << a << b;
    }

    void calculate_lcm() {
        int local_a = a.read();
        int local_b = b.read();
        int max_num = (local_a > local_b) ? local_a : local_b;
        int flag = 1;

        while (flag) {
            if (max_num % local_a == 0 && max_num % local_b == 0) {
                lcm_result.write(max_num);
                flag = 0; // Exit the loop after finding LCM
            } else {
                ++max_num;
            }
        }
    }
};

// Example top module to instantiate LcmCalculator

SC_MODULE(TopModule) {
    sc_signal<int> a_sig, b_sig;
    sc_signal<int> lcm_result_sig;

    LcmCalculator lcm_calc;

    SC_CTOR(TopModule) : lcm_calc("lcm_calc") {
        lcm_calc.a(a_sig);
        lcm_calc.b(b_sig);
        lcm_calc.lcm_result(lcm_result_sig);

        // Initialize inputs
        SC_THREAD(initialize_inputs);
    }

    void initialize_inputs() {
        a_sig = 15;
        b_sig = 20;
        wait(1, SC_NS); // Wait for a cycle to let the calculation happen
        cout << "LCM of " << a_sig.read() << " and " << b_sig.read() << " is " << lcm_result_sig.read() << endl;
        sc_stop(); // Stop simulation
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
