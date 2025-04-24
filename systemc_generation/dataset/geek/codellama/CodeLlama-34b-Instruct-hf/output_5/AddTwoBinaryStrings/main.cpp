
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    // Input signals
    sc_in< std::string > A;
    sc_in< std::string > B;

    // Output signal
    sc_out< std::string > sum;

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two strings
    void add() {
        // Swap the strings if necessary
        if (A.length() > B.length()) {
            std::swap(A, B);
        }

        // Calculate the difference in length
        int diff = B.length() - A.length();

        // Initialize the padding
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Pad A with zeros
        A = padding + A;

        // Initialize the result and carry
        std::string res;
        char carry = '0';

        // Loop through each digit
        for (int i = A.length() - 1; i >= 0; i--) {
            // If both digits are '1', add and set carry bit
            if (A[i] == '1' && B[i] == '1') {
                res.push_back('1');
                carry = '1';
            }

            // If both digits are '0', add and set carry bit
            else if (A[i] == '0' && B[i] == '0') {
                res.push_back('0');
                carry = '1';
            }

            // If both digits are different, add and set carry bit
            else if (A[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '0';
                } else {
                    res.push_back('1');
                    carry = '1';
                }
            }
        }

        // Add carry digit if necessary
        if (carry == '1') {
            res.push_back(carry);
        }

        // Reverse result and remove leading zeroes
        std::reverse(res.begin(), res.end());
        res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](char c) { return c != '0'; }));

        // Assign result to output signal
        sum.write(res);
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
