
#include <systemc.h>
#include <string>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Initialize the padding string with zeroes
        std::string padding;
        for (int i = 0; i < A.length(); i++) {
            padding.push_back('0');
        }

        // Pad the shorter string with zeroes
        if (A.length() > B.length()) {
            A = padding + A;
        } else {
            B = padding + B;
        }

        // Initialize the result string
        std::string res;
        char carry = '0';

        // Iterate through the characters of the concatenated string in reverse order
        for (int i = A.length() - 1; i >= 0; i--) {
            // If the current character of the concatenated string is '1' and the corresponding character of the other string is also '1', set the current character of the result string to '1' and set the carry flag to '1'.
            if (A[i] == '1' && B[i] == '1') {
                res.push_back('1');
                carry = '1';
            }
            // If the current character of the concatenated string is '0' and the corresponding character of the other string is also '0', set the current character of the result string to '0' and set the carry flag to '0'.
            else if (A[i] == '0' && B[i] == '0') {
                res.push_back('0');
                carry = '0';
            }
            // If the current character of the concatenated string is neither '1' nor '0' and the corresponding character of the other string is '1', set the current character of the result string to '0' and set the carry flag to '1'.
            else if (A[i] != B[i]) {
                res.push_back('0');
                carry = '1';
            }
        }

        // If at the end the carry flag is set, add it to the result string
        if (carry == '1') {
            res.push_back(carry);
        }

        // Remove leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0') index++;
        sum.write(res.substr(index));
    }
};


SC_MODULE(Testbench) {
    sc_signal<std::string> a; // Signal for the first binary string
    sc_signal<std::string> b; // Signal for the second binary string
    sc_signal<std::string> sum; // Signal for the sum of the binary strings

    BinaryAdder adder_inst; // Instance of the BinaryAdder module

    // Constructor
    SC_CTOR(Testbench) : adder_inst("adder") {
        // Connect signals to ports
        adder_inst.A(a);
        adder_inst.B(b);
        adder_inst.sum(sum);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Simple addition
        a = "1101"; b = "100";
        wait(1, SC_NS); // Wait for the adder to process
        assert(sum.read() == "10001");
        std::cout << "Sum: " << sum.read() << std::endl;

        // Test case 2: Different lengths
        a = "1010"; b = "11010";
        wait(1, SC_NS);
        assert(sum.read() == "100100");
        std::cout << "Sum: " << sum.read() << std::endl;

        // Test case 3: Leading zeros in result
        a = "0"; b = "0";
        wait(1, SC_NS);
        assert(sum.read() == "0");
        std::cout << "Sum: " << sum.read() << std::endl;

        // Test case 4: Different values
        a = "1111"; b = "1";
        wait(1, SC_NS);
        assert(sum.read() == "10000");
        std::cout << "Sum: " << sum.read() << std::endl;

        // Print success message
        std::cout << "All tests passed successfully." << std::endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
