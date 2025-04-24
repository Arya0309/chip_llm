
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first string
    sc_in<std::string> B;  // Input port for the second string
    sc_out<std::string> sum;  // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        // Swap the strings if necessary
        if (A.read().length() > B.read().length()) {
            sc_in<std::string> temp = A;
            A = B;
            B = temp;
        }

        // Calculate the difference in length
        int diff = B.read().length() - A.read().length();

        // Initialize the padding string
        std::string padding;
        for (int i = 0; i < diff; i++) {
            padding.push_back('0');
        }

        // Add the padding to the shorter string
        A.write(padding + A.read());

        // Initialize the result and carry strings
        std::string res, carry;
        carry.push_back('0');

        // Iterate through each digit in reverse order
        for (int i = A.read().length() - 1; i >= 0; i--) {
            // If both digits are '1', add '1' to res and set carry to '1'
            if (A.read()[i] == '1' && B.read()[i] == '1') {
                res.push_back('1');
                carry.push_back('1');
            }

            // If both digits are '0', add '0' to res and set carry to '0'
            if (A.read()[i] == '0' && B.read()[i] == '0') {
                res.push_back('0');
                carry.push_back('0');
            }

            // If the digits are different, add carry to res and flip the carry bit
            if (A.read()[i] != B.read()[i]) {
                res.push_back(carry[i]);
                carry.push_back(carry[i] == '1' ? '0' : '1');
            }
        }

        // If the result has a leading zero, skip it and continue
        int j = 0;
        while (j < res.length() && res[j] == '0') {
            j++;
        }
        if (j > 0) {
            res.erase(j - 1, 1);
        }

        // Reverse the result and remove any leading zeros
        std::reverse(res.begin(), res.end());
        j = 0;
        while (j < res.length() && res[j] == '0') {
            j++;
        }
        if (j > 0) {
            res.erase(j - 1, j);
        }

        // Output the sum
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
