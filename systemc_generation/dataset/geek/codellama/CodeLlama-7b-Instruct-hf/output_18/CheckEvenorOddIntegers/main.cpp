
#include <systemc.h>

// Module to check if a number is even or odd using modulo

SC_MODULE(EvenOddChecker) {
    sc_in<int> number_in; // input port for the number
    sc_out<string> result_out; // output port for the result

    // Constructor
    SC_CTOR(EvenOddChecker) {
        // Process to check if the input number is even or odd
        SC_METHOD(check_number);
        sensitive << number_in;
    }

    // Method to check if the number is even or odd
    void check_number() {
        int number = number_in.read(); // Read the input number from the port

        // If the number is even
        if (number % 2 == 0) {
            result_out.write("Even"); // Print "Even"
        }
        // If the number is odd
        else {
            result_out.write("Odd"); // Print "Odd"
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
