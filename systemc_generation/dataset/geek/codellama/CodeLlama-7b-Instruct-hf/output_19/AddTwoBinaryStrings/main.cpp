scss
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;
    sc_in<std::string> B;
    sc_out<std::string> sum;

    SC_METHOD(add);
    sensitive << A << B;

    void add() {
        // Calculate the difference between the lengths of A and B
        int diff = B.length() - A.length();

        // Initialize a padding string as zeroes of length diff
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Append the padded version of A to the beginning of B
        B = padding + A;

        // Initialize a string variable to hold the result
        std::string res;
        char carry = '0';

        // Iterate through the combined string in reverse order
        for (int i = A.length() - 1; i >= 0; i--) {
            // If the current character is '1' and the corresponding character in B is '1', add a '1' to the result and set the carry variable to '1'
            if (A[i] == '1' && B[i] == '1') {
                res.push_back('1');
                carry = '1';
            }
            // If the current character is '0' and the corresponding character in B is '0', add a '0' to the result and set the carry variable to '0'
            else if (A[i] == '0' && B[i] == '0') {
                res.push_back('0');
                carry = '0';
            }
            // If the current character is not equal to the corresponding character in B, add a '1' to the result and set the carry variable to '1'
            else if (A[i] != B[i]) {
                res.push_back('1');
                carry = '1';
            }
        }

        // If the carry variable is '1', add a '1' to the end of the result
        if (carry == '1') {
            res.push_back('1');
        }

        // Reverse the result string
        reverse(res.begin(), res.end());

        // Return the result
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
