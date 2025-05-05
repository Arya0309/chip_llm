
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        std::string a = A.read();
        std::string b = B.read();

        // Ensure a is the longer string
        if (a.length() < b.length()) {
            a.swap(b);
        }

        std::string result;
        char carry = '0';

        // Perform binary addition
        for (int i = a.length() - 1; i >= 0; --i) {
            char bitA = a[i] == '1' ? '1' : '0';
            char bitB = i >= b.length() ? '0' : b[i] == '1' ? '1' : '0';

            if (bitA == '1' && bitB == '1') {
                if (carry == '1') {
                    result.push_back('1');
                } else {
                    result.push_back('0');
                    carry = '1';
                }
            } else if (bitA == '0' && bitB == '0') {
                if (carry == '1') {
                    result.push_back('1');
                    carry = '0';
                } else {
                    result.push_back('0');
                }
            } else {
                if (carry == '1') {
                    result.push_back('0');
                } else {
                    result.push_back('1');
                }
            }
        }

        // Handle remaining carry
        if (carry == '1') {
            result.push_back('1');
        }

        // Reverse the result
        std::reverse(result.begin(), result.end());

        // Remove leading zeros
        size_t first_non_zero = result.find_first_not_of('0');
        if (first_non_zero != std::string::npos) {
            result = result.substr(first_non_zero);
        } else {
            result = "0";
        }

        sum.write(result);
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
