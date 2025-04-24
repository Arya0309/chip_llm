
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    // Input ports
    sc_in<string> A;
    sc_in<string> B;
    sc_out<string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Check if the length of A is greater than the length of B
        if (A.length() > B.length()) {
            // Swap A and B
            swap(A, B);
        }

        // Calculate the difference between the lengths of A and B
        int diff = B.length() - A.length();

        // Create a padding string with the necessary number of zeroes
        // to prefix the shorter string A if necessary
        string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // If the shorter string A needs to be padded, pad it
        if (padding != "") {
            A = padding + A;
        }

        // Variables to keep track of the result and carry
        string res;
        char carry = '0';

        // Iterate on the longer string A from right to left
        for (int i = A.length() - 1; i >= 0; i--) {
            // If the current character in A is 1 and the current character in B is 1
            if (A[i] == '1' && B[i] == '1') {
                // If the carry is 1, add 1 to the result string and set the carry to 1
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                // Otherwise, add 0 to the result string and set the carry to 0
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            // If the current character in A is 0 and the current character in B is 0
            } else if (A[i] == '0' && B[i] == '0') {
                // If the carry is 1, add 0 to the result string and set the carry to 0
                if (carry == '1') {
                    res.push_back('0');
                    carry = '0';
                // Otherwise, add 0 to the result string and set the carry to 0
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            // If the current character in A is not equal to the current character in B
            } else if (A[i] != B[i]) {
                // If the carry is 1, add 0 to the result string and set the carry to 1
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                // Otherwise, add 1 to the result string and set the carry to 0
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        // If at the end there is a carry, add it to the result
        if (carry == '1') {
            res.push_back(carry);
        }

        // Reverse the result string
        reverse(res.begin(), res.end());

        // Remove leading zeroes from the result string
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }

        sum = res.substr(index);
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
