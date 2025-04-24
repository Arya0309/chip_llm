c
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    // Input ports
    sc_in<std::string> A;
    sc_in<std::string> B;

    // Output port
    sc_out<std::string> sum;

    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two strings
    void add() {
        // Calculate the difference in length
        int diff = A.length() - B.length();

        // Pad the shorter string with zeroes
        sc_string padding = sc_pad(A, diff, '0');

        // Initialize the result string
        sc_string res = "";

        // Perform recursive addition
        for (int i = 0; i < A.length(); i++) {
            // If both bits are set to 1 or both are set to 0
            if (A[i] == '1' && B[i] == '1') {
                // Add the bits and set the carry flag
                res.push_back('1');
                carry_flag = '1';
            } else if (A[i] == '0' && B[i] == '0') {
                // Add the bits and clear the carry flag
                res.push_back('0');
                carry_flag = '0';
            } else if (A[i] != B[i]) {
                // Add the bits and toggle the carry flag
                res.push_back(carry_flag == '1' ? '1' : '0');
                carry_flag = carry_flag == '1' ? '0' : '1';
            }
        }

        // Add any remaining carry flags
        if (carry_flag == '1') {
            res.push_back(carry_flag);
        }

        // Reverse the result
        res.reverse();

        // Remove leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }

        // Return the result
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
