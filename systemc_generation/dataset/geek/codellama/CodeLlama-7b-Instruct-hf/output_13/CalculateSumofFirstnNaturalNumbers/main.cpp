
#include <systemc.h>

// Module to compute the sum of the first n natural numbers

SC_MODULE(SumCalculator) {
    sc_in<unsigned int> n;    // Input port for the value of n
    sc_out<unsigned int> result;  // Output port for the sum

    // Declaration of the findSum function
    sc_method<unsigned int> findSum;

    // Constructor
    SC_CTOR(SumCalculator) {
        // Connect the input ports to the findSum method
        n.register_in(findSum);
        // Connect the result output port to the findSum method
        findSum.register_out(result);
    }

    // Natural numbers from 1 to n
    sc_method<unsigned int> naturalNumbers;

    // For loop to iterate from 1 to n and incrementally add the i-th natural number to sum
    sc_process_group naturalNumbersBlock;

    sc_method<unsigned int> sum;

    // Natural numbers from 1 to n
    sc_method<unsigned int> result;

    // Constructor
    SC_CTOR(SumCalculator) {
        // Connect the input ports to the findSum method
        n.register_in(findSum);
        // Connect the result output port to the findSum method
        findSum.register_out(result);
    }

    // Calculate the sum of the first n natural numbers
    void findSum() {
        unsigned int i = 0;
        sum = 0;
        for (i=1; i<n; i++) {
            sum = sum + i;
        }
        result = sum;
    }
};


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
