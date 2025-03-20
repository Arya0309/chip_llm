
#include <systemc.h>

// Module to reverse digits of an integer
SC_MODULE(ReverseDigits) {
    sc_in<int> num_in;  // Input port for the number
    sc_out<int> num_out; // Output port for the reversed number

    // Constructor
    SC_CTOR(ReverseDigits) {
        // Process to reverse the digits
        SC_METHOD(reverse);
        sensitive << num_in;
    }

    // Method to reverse the digits of the input number
    void reverse() {
        int num = num_in.read();
        int rev_num = 0;
        while (num > 0) {
            rev_num = rev_num * 10 + num % 10;
            num = num / 10;
        }
        num_out.write(rev_num);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> num_sig;       // Signal for the input number
    sc_signal<int> rev_num_sig;   // Signal for the reversed number

    // Instance of the ReverseDigits module
    ReverseDigits reverse_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : reverse_inst("reverse_inst") {
        // Connect signals to module ports
        reverse_inst.num_in(num_sig);
        reverse_inst.num_out(rev_num_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: num = 4562 (expected reversed number = 2654)
        num_sig.write(4562);
        wait(1, SC_NS); // Wait for the process to update
        int result = rev_num_sig.read();
        std::cout << "Reverse of " << num_sig.read() << " is " << result << std::endl;
        assert(result == 2654);

        // Additional test case: num = 12345 (expected reversed number = 54321)
        num_sig.write(12345);
        wait(1, SC_NS);
        result = rev_num_sig.read();
        std::cout << "Reverse of " << num_sig.read() << " is " << result << std::endl;
        assert(result == 54321);

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
