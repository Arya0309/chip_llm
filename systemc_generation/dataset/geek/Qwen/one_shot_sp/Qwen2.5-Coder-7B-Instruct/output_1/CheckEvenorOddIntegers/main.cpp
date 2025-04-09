
#include <systemc.h>


SC_MODULE(EvenOddChecker) {
    sc_out<bool> result_out; // Output port for the result (true for even, false for odd)

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Initialize the number to be checked
        int n = 11;

        // Process to determine if the number is even or odd
        SC_THREAD(check_number);
    }

    // Thread to perform the check
    void check_number() {
        int n = 11; // Hardcoded number to check
        if (n % 2 == 0) {
            result_out.write(true); // Write true to indicate even
        } else {
            result_out.write(false); // Write false to indicate odd
        }
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
