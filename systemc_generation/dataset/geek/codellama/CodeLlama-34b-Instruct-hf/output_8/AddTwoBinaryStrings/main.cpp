
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<string> A;  // Input port for the first binary string
    sc_in<string> B;  // Input port for the second binary string
    sc_out<string> sum;  // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Method to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add();
        // Check if A is longer than B
        if (A.length() > B.length()) {
            // Swap the strings
            std::string temp = A;
            A = B;
            B = temp;
        }

        // Calculate the difference in length between the two strings
        int diff = B.length() - A.length();

        // Pad the shorter string with leading zeroes
        std::string padding(diff, '0');
        A = padding + A;

        // Add the two strings bit by bit, starting from the LSB
        std::string res;
        char carry = '0';
        for (int i = A.length() - 1; i >= 0; i--) {
            // Check if A and B have the same value at the current bit
            if (A[i] == B[i]) {
                // Add the two bits and the carry
                res.push_back(A[i]);
                carry = '0';
            } else {
                // Add the carry and the current bit
                res.push_back(carry);
                carry = '1';
            }
        }

        // Check if there is a carry bit left over
        if (carry == '1') {
            res.push_back(carry);
        }

        // Remove any leading zeroes from the result string
        int index = 0;
        while (res[index] == '0') {
            index++;
        }
        res = res.substr(index);

        // Write the result to the output port
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
