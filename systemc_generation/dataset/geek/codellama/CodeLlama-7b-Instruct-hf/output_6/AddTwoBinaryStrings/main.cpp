scss
#include <systemc.h>

// Module to add two binary strings and return the sum

SC_MODULE(BinaryAdder) {
    sc_in<string> A;  // Input port for the first binary string
    sc_in<string> B;  // Input port for the second binary string
    sc_out<string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Declare variables for padding and carryover
        string padding;
        char carry = '0';

        // Calculate the difference between the lengths of the two strings
        int diff = B.length() - A.length();

        // If the length of the first string is greater, pad the second string
        if (A.length() > B.length()) {
            padding = string(diff, '0');
            B = padding + B;
        }

        // Add the elements of the two strings, keeping track of carryover
        string res;
        for (int i = A.length() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            } else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '0';
                } else {
                    res.push_back('1');
                    carry = '1';
                }
            } else if (A[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '0';
                } else {
                    res.push_back('1');
                    carry = '1';
                }
            }
        }

        // If there was a carryover, add it to the result
        if (carry == '1') {
            res.push_back('1');
        }

        // Reverse the result and remove leading zeroes
        string reversed_res(res.rbegin(), res.rend());
        int index = 0;
        while (index + 1 < reversed_res.length() && reversed_res[index] == '0') {
            index++;
        }
        sum.write(reversed_res.substr(index));
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
