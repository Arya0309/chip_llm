
#include <systemc.h>


SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in;  // Input signal for the number to check
    sc_out<int> result_out; // Output signal for the result (0 for even, 1 for odd)

    void check_even_odd() {
        // Wait until there's a change in the input signal
        wait();

        // Read the input value
        int n = number_in.read();

        // Check if the number is even or odd
        if (n % 2 == 0) {
            result_out.write(0); // Write 0 to indicate even
        } else {
            result_out.write(1); // Write 1 to indicate odd
        }
    }

    SC_CTOR(EvenOddChecker) {
        SC_METHOD(check_even_odd);
        sensitive << number_in; // Sensitivity list to trigger on changes to number_in
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
