
#include <systemc.h>

// Module that checks if a number is even or odd

SC_MODULE(EvenOddChecker) {
    // Input port for the number to check
    sc_in<int> number_in;
    // Output port for the result (1 for Even, 0 for Odd)
    sc_out<int> result_out;

    SC_CTOR(EvenOddChecker) {
        // Use an SC_METHOD to perform the check
        SC_METHOD(check_even_odd);
        sensitive << number_in;
    }

    // Method that checks if the number is even or odd
    void check_even_odd() {
        int n = number_in.read();
        // If n is completely divisible by 2
        if (n % 2 == 0) {
            result_out.write(1); // Write 1 to indicate Even
        } else {
            result_out.write(0); // Write 0 to indicate Odd
        }
    }
};

// Example testbench to demonstrate the usage of EvenOddChecker


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
