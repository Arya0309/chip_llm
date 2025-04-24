
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A; // Input port for the first binary string
    sc_in<std::string> B; // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Initialize the result string
        std::string res;

        // Check if the length of A is greater than the length of B
        if (A.length() > B.length()) {
            // Swap the strings
            std::string temp = A;
            A = B;
            B = temp;
        }

        // Calculate the difference in length
        int diff = B.length() - A.length();

        // Initialize the padding string
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Add the padding to the shorter string
        A = padding + A;

        // Initialize the carry variable
        char carry = '0';

        // Perform the addition
        for (int i = A.length() - 1; i >= 0; i--) {
            // If the two strings have the same digit
            if (A[i] == B[i]) {
                // If there is a carry, append a 1 to the result
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            } else {
                // If the two strings have different digits
                if (carry == '1') {
                    // If the two strings have different digits and there is a carry, append a 1 to the result
                    res.push_back('1');
                    carry = '0';
                } else {
                    // If the two strings have different digits and there is no carry, append a 0 to the result
                    res.push_back('0');
                    carry = '1';
                }
            }
        }

        // If there is a carry, append a 1 to the result
        if (carry == '1') {
            res.push_back('1');
        }

        // Reverse the result
        std::reverse(res.begin(), res.end());

        // Remove any leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }

        // Update the output port
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
