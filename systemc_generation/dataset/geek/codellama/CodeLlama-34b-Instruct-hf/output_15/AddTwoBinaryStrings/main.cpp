
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> a;  // Input port for the first number
    sc_in<std::string> b;  // Input port for the second number
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(add);
        sensitive << a << b;
    }

    // Method to perform binary addition
    void add() {
        // Get the lengths of the input strings
        int lenA = a.length();
        int lenB = b.length();

        // If the length of a is greater than the length of b, swap the strings
        if (lenA > lenB) {
            std::string temp = a;
            a = b;
            b = temp;
        }

        // Calculate the difference in length between the strings
        int diff = lenB - lenA;

        // Initialize the padding string with diff number of '0's
        std::string padding;
        padding.fill('0', diff);

        // Pad the shorter string with '0's
        a = padding + a;

        // Initialize the result string
        std::string res;
        res.resize(lenA);

        // Initialize the carry variable
        char carry = '0';

        // Iterate over each character of the strings from right to left
        for (int i = lenA - 1; i >= 0; i--) {
            if (a[i] == '1' && b[i] == '1') {
                // If both characters are '1', check the value of carry
                if (carry == '1') {
                    // If carry is '1', set the next character of res to '1' and set carry to '1'
                    res[i] = '1';
                    carry = '1';
                } else {
                    // If carry is '0', set the next character of res to '0' and set carry to '1'
                    res[i] = '0';
                    carry = '1';
                }
            } else if (a[i] == '0' && b[i] == '0') {
                // If both characters are '0', check the value of carry
                if (carry == '1') {
                    // If carry is '1', set the next character of res to '1' and set carry to '0'
                    res[i] = '1';
                    carry = '0';
                } else {
                    // If carry is '0', set the next character of res to '0' and set carry to '0'
                    res[i] = '0';
                    carry = '0';
                }
            } else if (a[i] != b[i]) {
                // If the characters are not the same, set the next character of res to the non-zero character
                res[i] = a[i] != '0' ? a[i] : b[i];

                // Set carry to the sum of the carry-over and the non-zero character modulo 2
                carry = (carry + (a[i] != '0' ? 1 : 0) + (b[i] != '0' ? 1 : 0)) % 2;
            }
        }

        // If there is a carry-over value at the end, append it to the result string
        if (carry == '1') {
            res += carry;
        }

        // If the result string has leading '0's, remove them
        while (res[0] == '0' && res.length() > 1) {
            res.erase(res.begin());
        }

        // Write the result to the sum output port
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
