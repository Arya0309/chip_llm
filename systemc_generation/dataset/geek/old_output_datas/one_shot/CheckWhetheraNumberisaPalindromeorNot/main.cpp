
#include <systemc.h>
#include <string>

// Module to check if a string is a palindrome
SC_MODULE(PalindromeChecker) {
    sc_in<bool> clk; // Clock signal
    sc_in<bool> reset; // Reset signal
    sc_in<std::string> input_str; // Input string
    sc_out<bool> result; // Output result (true if palindrome)

    std::string current_str; // Internal storage for the current string
    bool is_palindrome; // Result flag

    // Constructor
    SC_CTOR(PalindromeChecker) {
        // Register a method to check palindrome
        SC_METHOD(check);
        sensitive << clk.posedge_event() << reset.posedge_event();
        dont_initialize();
    }

    // Method to check if the input string is a palindrome
    void check() {
        if (reset.read()) {
            is_palindrome = false;
            result.write(false);
            current_str.clear();
        } else {
            current_str = input_str.read();
            is_palindrome = checkPalindrome(current_str);
            result.write(is_palindrome);
        }
    }

    // Helper function to check palindrome
    bool checkPalindrome(const std::string& str) {
        int len = str.length();
        for (int i = 0; i < len / 2; ++i) {
            if (str[i] != str[len - i - 1]) {
                return false;
            }
        }
        return true;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_clock clk; // Clock signal
    sc_signal<bool> reset; // Reset signal
    sc_signal<std::string> input_str; // Input string
    sc_signal<bool> result; // Result signal

    PalindromeChecker checker_inst; // Instance of the PalindromeChecker module

    // Constructor
    SC_CTOR(Testbench) : clk("clk", 10, SC_NS), checker_inst("checker") {
        // Connect signals to ports
        checker_inst.clk(clk);
        checker_inst.reset(reset);
        checker_inst.input_str(input_str);
        checker_inst.result(result);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Reset the system
        reset = true;
        wait(20, SC_NS);
        reset = false;
        wait(10, SC_NS);

        // Test case 1: Palindrome string
        input_str = "112233445566778899000000998877665544332211";
        wait(10, SC_NS);
        cout << "Input: " << input_str.read() << " -> Result: " << (result.read() ? "Yes" : "No") << endl;

        // Test case 2: Non-palindrome string
        input_str = "123456789";
        wait(10, SC_NS);
        cout << "Input: " << input_str.read() << " -> Result: " << (result.read() ? "Yes" : "No") << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
