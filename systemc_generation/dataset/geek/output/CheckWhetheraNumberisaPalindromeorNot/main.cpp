
#include <systemc.h>
#include <string>

// Module to check if a string is a palindrome
SC_MODULE(PalindromeChecker) {
    sc_in<std::string> input_string;  // Input port for the string to check
    sc_out<bool> is_palindrome;      // Output port indicating if the string is a palindrome

    // Constructor
    SC_CTOR(PalindromeChecker) {
        // Process to check palindrome
        SC_METHOD(check_palindrome);
        sensitive << input_string;
    }

    // Method to check if the string is a palindrome
    void check_palindrome() {
        std::string str = input_string.read();
        int len = str.length();
        bool result = true;

        for (int i = 0; i < len / 2; i++) {
            if (str[i] != str[len - i - 1]) {
                result = false;
                break;
            }
        }

        is_palindrome.write(result);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> str_sig;  // Signal for the string input
    sc_signal<bool> result_sig;      // Signal for the palindrome result

    // Instance of the PalindromeChecker module
    PalindromeChecker checker_inst;

    // Constructor
    SC_CTOR(Testbench)
    : checker_inst("checker_inst") {
        // Connect signals to module ports
        checker_inst.input_string(str_sig);
        checker_inst.is_palindrome(result_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: Given string
        str_sig.write("112233445566778899000000998877665544332211");
        wait(1, SC_NS); // Wait for the process to update
        bool result = result_sig.read();
        std::cout << "Is palindrome: " << (result ? "Yes" : "No") << std::endl;
        assert(result == true);

        // Additional test case: Non-palindrome string
        str_sig.write("HelloWorld");
        wait(1, SC_NS);
        result = result_sig.read();
        std::cout << "Is palindrome: " << (result ? "Yes" : "No") << std::endl;
        assert(result == false);

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
