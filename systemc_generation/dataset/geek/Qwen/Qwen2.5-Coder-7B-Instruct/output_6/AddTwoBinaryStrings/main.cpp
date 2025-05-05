
#include <systemc.h>
#include <string>
#include <algorithm>

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
        std::string a = A.read();
        std::string b = B.read();

        // Limit the length of the input strings
        const size_t max_length = 1024; // Adjust this value as needed
        if (a.length() > max_length) {
            a = a.substr(0, max_length);
        }
        if (b.length() > max_length) {
            b = b.substr(0, max_length);
        }

        // If A is longer than B, swap them
        if (a.length() > b.length()) {
            std::swap(a, b);
        }

        // Calculate the difference in lengths
        int diff = b.length() - a.length();

        // Initialize padding string
        std::string padding(diff, '0');

        // Pad A with zeros
        a = padding + a;

        std::string res;
        char carry = '0';

        // Iterate over the characters from right to left
        for (int i = a.length() - 1; i >= 0; i--) {
            if (a[i] == '1' && b[i] == '1') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            } else if (a[i] == '0' && b[i] == '0') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '0';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            } else if (a[i] != b[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        // Add carry if present
        if (carry == '1') {
            res.push_back(carry);
        }

        // Reverse the result
        std::reverse(res.begin(), res.end());

        // Remove leading zeros
        size_t index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }

        // Ensure the result does not exceed the maximum length
        if (res.length() > max_length) {
            res = res.substr(0, max_length);
        }

        // Write the result to the output port
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
