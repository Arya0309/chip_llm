
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the binary sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        std::string res;
        char carry = '0';

        // Ensure A is the shorter string
        if (A->length() > B->length()) {
            addBinary(B, A, res, carry);
        } else {
            addBinary(A, B, res, carry);
        }

        // Remove leading zeros
        size_t index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }
        sum.write(res.substr(index));
    }

    // Helper method to perform the actual addition
    void addBinary(const std::string& A, const std::string& B, std::string& res, char& carry) {
        int diff = B.length() - A.length();
        std::string padding(diff, '0');
        std::string paddedA = padding + A;

        for (int i = paddedA.length() - 1; i >= 0; i--) {
            if (paddedA[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            } else if (paddedA[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '0';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            } else if (paddedA[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        if (carry == '1') {
            res.push_back(carry);
        }

        reverse(res.begin(), res.end());
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
