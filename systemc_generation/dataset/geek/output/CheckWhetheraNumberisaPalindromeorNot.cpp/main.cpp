
#include <systemc.h>

// Module to check if a string is a palindrome
SC_MODULE(PalindromeChecker) {
    // Input port for the string
    sc_in<std::string> input_str;
    
    // Output port for the result
    sc_out<bool> is_palindrome;

    // Process to check palindrome
    void checkPalindromeProcess() {
        // Reading input string
        std::string str = input_str.read();
        
        // Calculating string length
        int len = str.length();
        
        bool result = true;
        
        // Traversing through the string up to half its length
        for (int i = 0; i < len / 2; i++) {
            // Comparing i-th character from starting and len-i-th character from end
            if (str[i] != str[len - i - 1]) {
                result = false;
                break;
            }
        }
        
        // Writing the result
        is_palindrome.write(result);
    }

    // Constructor to register the process
    SC_CTOR(PalindromeChecker) {
        SC_METHOD(checkPalindromeProcess);
        sensitive << input_str;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with PalindromeChecker
    sc_signal<std::string> input_str;
    sc_signal<bool> is_palindrome;

    // Instance of PalindromeChecker
    PalindromeChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing the input string
        input_str = "112233445566778899000000998877665544332211";

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the result
        if (is_palindrome.read())
            cout << "Yes" << endl;
        else
            cout << "No" << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.input_str(input_str);
        checker.is_palindrome(is_palindrome);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
