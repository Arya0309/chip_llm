
#include <systemc.h>


SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in;      // Input signal for the number to check
    sc_out<int> result_out;     // Output signal indicating Even (0) or Odd (1)

    SC_CTOR(EvenOddChecker) {
        SC_METHOD(check_even_odd);  // Register the method that will perform the check
        sensitive << number_in;    // Specify sensitivity to changes in the input signal
    }

    void check_even_odd() {
        int n = number_in.read();  // Read the input number
        int result = (n % 2 != 0); // Calculate result: 0 for Even, 1 for Odd
        result_out.write(result);  // Write the result to the output signal
    }
};



SC_MODULE(Testbench) {
    sc_signal<int> number_sig;   // Signal for the input number
    sc_signal<int> result_sig;   // Signal for the result

    EvenOddChecker checker_inst; // Instance of the EvenOddChecker module

    SC_CTOR(Testbench)
    : checker_inst("checker_inst")
    {
        checker_inst.number_in(number_sig);
        checker_inst.result_out(result_sig);
        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case 1: Number 11 (Odd)
        number_sig.write(11);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 1
        assert(result_sig.read() == 1);

        // Test case 2: Number 12 (Even)
        number_sig.write(12);
        wait(1, SC_NS);
        std::cout << result_sig.read() << std::endl; // Expected output: 0
        assert(result_sig.read() == 0);
        
        std::cout << "All tests pass successfully!" << std::endl;
        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
