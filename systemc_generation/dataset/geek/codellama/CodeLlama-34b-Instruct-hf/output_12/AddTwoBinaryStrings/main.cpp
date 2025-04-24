
#include <iostream>
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;
    sc_in<std::string> B;
    sc_out<std::string> sum;

    sc_CTOR(BinaryAdder) {
        // Add the two strings
        sum.write(addBinary(A.read(), B.read()));
    }

    // Function to add two binary strings
    std::string addBinary(std::string A, std::string B) {
        // If the length of A is greater than the length of B, pad A with leading zeros
        if (A.length() > B.length()) {
            B = std::string(A.length() - B.length(), '0') + B;
        }

        // Initialize the result and carry
        std::string res = "";
        char carry = '0';

        // Loop through each position in both A and B
        for (int i = A.length() - 1; i >= 0; i--) {
            // If both A and B are '1' at this position, generate a carry
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.insert(0, "0");
                } else {
                    res.insert(0, "1");
                    carry = '0';
                }
            }

            // If both A and B are '0' at this position, generate a carry
            if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.insert(0, "1");
                } else {
                    res.insert(0, "0");
                    carry = '1';
                }
            }

            // If the characters at this position are different, generate a carry
            if (A[i] != B[i]) {
                if (carry == '1') {
                    res.insert(0, "0");
                } else {
                    res.insert(0, "1");
                    carry = '1';
                }
            }

            // If the characters are the same, do not generate a carry
            else {
                res.insert(0, "0");
            }
        }

        // If there is a remaining carry, generate a carry
        if (carry == '1') {
            res.insert(0, "1");
        }

        return res;
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
