
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<sc_string> A;  // Input port for the first binary string
    sc_in<sc_string> B;  // Input port for the second binary string
    sc_out<sc_string> sum; // Output port for the sum of the two binary strings

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(binary_add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void binary_add() {
        // Get the input strings
        sc_string a = A.read();
        sc_string b = B.read();

        // Ensure a is the shorter string
        if (a.length() > b.length()) {
            sc_string temp = a;
            a = b;
            b = temp;
        }

        // Calculate the length difference
        int diff = b.length() - a.length();

        // Initialize padding string
        sc_string padding(diff, '0');

        // Append padding to the shorter string
        a = padding + a;

        // Initialize result string and carry variable
        sc_string res;
        char carry = '0';

        // Iterate over characters from right to left
        for (int i = a.length() - 1; i >= 0; i--) {
            if (a[i] == '1' && b[i] == '1') {
                if (carry == '1') {
                    res += '1';
                    carry = '1';
                } else {
                    res += '0';
                    carry = '1';
                }
            } else if (a[i] == '0' && b[i] == '0') {
                if (carry == '1') {
                    res += '1';
                    carry = '0';
                } else {
                    res += '0';
                    carry = '0';
                }
            } else {
                if (carry == '1') {
                    res += '0';
                    carry = '1';
                } else {
                    res += '1';
                    carry = '0';
                }
            }
        }

        // Add final carry if present
        if (carry == '1') {
            res += '1';
        }

        // Reverse the result string
        std::reverse(res.begin(), res.end());

        // Remove leading zeros
        size_t index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }
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
