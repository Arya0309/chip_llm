scss
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<string> A;     // Input port for the first binary string
    sc_in<string> B;     // Input port for the second binary string
    sc_out<string> Sum;  // Output port for the result

    SC_CTOR(BinaryAdder) {
        // Addition process
        SC_METHOD(binary_add)
        sensitive << A << B;
    }

    void binary_add() {
        string res;
        char carry = '0';

        // Pad the shorter string with zeros
        if (A.length() > B.length()) {
            int diff = A.length() - B.length();
            string padding;
            for (int i = 0; i < diff; i++)
                padding.push_back('0');
            B = padding + B;
        } else {
            int diff = B.length() - A.length();
            string padding;
            for (int i = 0; i < diff; i++)
                padding.push_back('0');
            A = padding + A;
        }

        // Add the strings together
        for (int i = A.length() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            } else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '0';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            } else if (A[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        // If carry, add it to the result
        if (carry == '1')
            res.push_back(carry);

        // Reverse and remove leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0')
            index++;
        Sum.write(res.substr(index));
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
