
#include <systemc.h>
...

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum;  // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Check if A is longer than B
        if (A.size() > B.size()) {
            B.swap(A);
        }

        // Calculate the difference in length between A and B
        int diff = B.size() - A.size();

        // Initialize the padding string
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Add A and B with padding
        std::string res = padding + A + B;

        // Calculate the carry flag
        char carry = '0';
        for (int i = A.size() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                // Add the two digits and set the carry flag
                carry = '1';
            } else if (A[i] == '0' && B[i] == '0') {
                // Add the two digits and clear the carry flag
                carry = '0';
            } else if (carry == '1') {
                // Add the two digits and toggle the carry flag
                carry = (A[i] == B[i]) ? '0' : '1';
            }
        }

        // If there is a carry, add it to the result
        if (carry == '1') {
            res.push_back(carry);
        }

        // Reverse the result
        std::reverse(res.begin(), res.end());

        // Remove leading zeroes
        int index = 0;
        while (index + 1 < res.size() && res[index] == '0') {
            index++;
        }
        res = res.substr(index);

        // Output the result
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
