
#include <systemc.h>

// SystemC module to check if the number is even or odd

SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in;  // Input port for the number
    sc_out<bool> result_out; // Output port for the result (true for even, false for odd)

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Process to determine if the number is even or odd
        SC_METHOD(check_even_odd);
        sensitive << number_in;
    }

    // Method to check if the number is even or odd
    void check_even_odd() {
        if (number_in.read() % 2 == 0) {
            result_out.write(true); // Even
        } else {
            result_out.write(false); // Odd
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
