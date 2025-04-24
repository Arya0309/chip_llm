scss
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Check if A is longer than B
        if (A.length() > B.length()) {
            // Swap A and B
            std::string temp = A;
            A = B;
            B = temp;
        }

        // Pad A with leading zeroes if necessary
        if (A.length() < B.length()) {
            std::string padding(B.length() - A.length(), '0');
            A = padding + A;
        }

        // Initialize carry flag
        bool carry = false;

        // Iterate over characters of A and B from right to left
        std::string res;
        for (int i = A.length() - 1; i >= 0; i--) {
            char a = A[i];
            char b = B[i];

            // Calculate the sum of the current characters
            if (a == '1' && b == '1') {
                if (carry) {
                    res.push_back('1');
                    carry = true;
                } else {
                    res.push_back('0');
                    carry = false;
                }
            } else if (a == '0' && b == '0') {
                if (carry) {
                    res.push_back('1');
                    carry = false;
                } else {
                    res.push_back('0');
                    carry = false;
                }
            } else if (a != b) {
                if (carry) {
                    res.push_back('0');
                    carry = true;
                } else {
                    res.push_back('1');
                    carry = false;
                }
            }
        }

        // If there is a carry at the end, add it to the result
        if (carry) {
            res.push_back('1');
        }

        // Reverse and remove leading zeroes
        std::reverse(res.begin(), res.end());
        int index = 0;
        while (index < res.length() && res[index] == '0') {
            index++;
        }
        res = res.substr(index);

        // Assign the result to the output port
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
