systemC
#include <systemc.h>

// Module that checks if an integer is even or odd using the modulo operator

SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in; // Input port for the number to be tested
    sc_out<string> result_out; // Output port for the result: "Even" or "Odd"

    // Method to check if the number is even or odd
    void check() {
        int n = number_in.read(); // Read the input number

        if (n % 2 == 0) {
            result_out.write("Even"); // Print "Even" if the input is even
        } else {
            result_out.write("Odd"); // Print "Odd" if the input is odd
        }
    }

    // Constructor
    SC_CTOR(EvenOddChecker) {
        SC_METHOD(check); // Call the check method
        sensitive << number_in; // Make the method sensitive to changes in the input
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
