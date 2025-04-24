
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    // Input signals for the two binary strings
    sc_in<std::string> A;
    sc_in<std::string> B;

    // Output signal for the sum of the two binary strings
    sc_out<std::string> sum;

    // Process to perform the addition
    SC_METHOD(add);
    sensitive << A << B;

    void add() {
        // Declare variables to store the lengths of the two strings
        int A_len = A.length();
        int B_len = B.length();

        // Check if the length of A is greater than the length of B
        if (A_len > B_len) {
            // If so, swap the strings by calling the same function with the arguments swapped
            sum.write(addBinary(B, A));
            return;
        }

        // Calculate the difference between the lengths of the two strings
        int diff = B_len - A_len;

        // Declare a variable to store the padding string of zeros
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Add the padding string to the front of A
        A = padding + A;

        // Declare variables to store the result string and the carry
        std::string res;
        char carry = '0';

        // Iterate through every character of the two strings from the last character to the first character
        for (int i = A_len - 1; i >= 0; i--) {
            // If both characters are '1', add 1 to the carry
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            }

            // If both characters are '0', add 0 to the carry
            else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '0';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            }

            // If the characters are different, add 0 to the carry
            else if (A[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        // Check if there is any carry at the end
        if (carry == '1') {
            res.push_back('1');
        }

        // Reverse the result string
        std::reverse(res.begin(), res.end());

        // Remove leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }

        // Output the result string
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
