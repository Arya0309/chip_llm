
#include <systemc.h>

// Module to calculate the sum of first n natural numbers
SC_MODULE(SumCalculator) {
    sc_in<unsigned int> n;      // Input port for the number of natural numbers
    sc_out<unsigned int> result; // Output port for the sum

    // Constructor
    SC_CTOR(SumCalculator) {
        // Process to compute the sum
        SC_METHOD(compute_sum);
        sensitive << n;
    }

    // Method to compute the sum
    void compute_sum() {
        unsigned int sum = 0;
        for (unsigned int i = 1; i <= n.read(); i++) {
            sum += i;
        }
        result.write(sum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<unsigned int> n_sig;    // Signal for the number of natural numbers
    sc_signal<unsigned int> result_sig; // Signal for the sum result

    // Instance of the SumCalculator module
    SumCalculator sum_calc_inst;

    // Constructor
    SC_CTOR(Testbench) : sum_calc_inst("sum_calc_inst") {
        // Connect signals to module ports
        sum_calc_inst.n(n_sig);
        sum_calc_inst.result(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: n = 5 (expected sum = 15)
        n_sig.write(5);
        wait(1, SC_NS); // Wait for the process to update
        unsigned int result = result_sig.read();
        std::cout << "Sum of first " << n_sig.read() << " natural numbers: " << result << std::endl;
        assert(result == 15);

        // Additional test case: n = 10 (expected sum = 55)
        n_sig.write(10);
        wait(1, SC_NS);
        result = result_sig.read();
        std::cout << "Sum of first " << n_sig.read() << " natural numbers: " << result << std::endl;
        assert(result == 55);

        // End simulation after successful test
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
