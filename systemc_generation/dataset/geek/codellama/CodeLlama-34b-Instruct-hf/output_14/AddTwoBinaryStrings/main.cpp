
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input 1
    sc_in<std::string> B;  // Input 2
    sc_out<std::string> sum;  // Output sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process for addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    void add() {
        // Swap inputs if necessary
        if (A.length() > B.length()) {
            A.push_back('0');
            string temp = A;
            A = B;
            B = temp;
        }

        // Pad the shorter input with leading zeroes
        int diff = B.length() - A.length();
        string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }
        A = padding + A;

        // Perform bitwise addition
        string result;
        char carry = '0';
        for (int i = A.length() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    result.push_back('1');
                    carry = '1';
                } else {
                    result.push_back('0');
                    carry = '1';
                }
            } else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    result.push_back('1');
                    carry = '0';
                } else {
                    result.push_back('0');
                    carry = '0';
                }
            } else if (A[i] != B[i]) {
                if (carry == '1') {
                    result.push_back('0');
                    carry = '1';
                } else {
                    result.push_back('1');
                    carry = '0';
                }
            }
        }

        // Check for remaining carry values
        if (carry == '1') {
            result.push_back(carry);
        }

        // Remove leading zeroes
        int index = 0;
        while (index + 1 < result.length() && result[index] == '0') {
            index++;
        }

        // Assign the output
        sum.write(result.substr(index));
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
