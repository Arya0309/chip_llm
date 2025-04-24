c++
#include <systemc.h>


SC_MODULE(EvenOddChecker) {
    // Input port for integer input
    sc_in<int> number_in;
    // Output port for string output
    sc_out<sc_string> result_out;

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Nothing to do here
    }

    // Method to check if the input is even or odd
    void checkInput() {
        // Check if the input is divisible by 2
        if (number_in.read() % 2 == 0) {
            // If the input is even, output "Even"
            result_out.write("Even");
        } else {
            // If the input is not even, output "Odd"
            result_out.write("Odd");
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
