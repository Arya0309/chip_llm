scss
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for string A
    sc_in<std::string> B;  // Input port for string B
    sc_out<std::string> sum; // Output port for the sum

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the strings
    void add() {
        // Initialize carry bit
        int carry = 0;

        // Iterate through each character of A and B
        for (int i = A.length(); i >= 0; i--) {
            char a = A.at(i);
            char b = B.at(i);

            // Check if there is a carry bit
            if (carry == 1) {
                // Add the carry bit to the result
                sum.append(carry);
                carry = 0;
            }

            // Check if both characters are ones
            if (a == '1' && b == '1') {
                // Set sum to one and reset carry bit
                sum.append('1');
                carry = 0;
            }

            // Check if both characters are zeroes
            else if (a == '0' && b == '0') {
                // Set sum to zero and reset carry bit
                sum.append('0');
                carry = 0;
            }

            // Check if characters are different
            else {
                // Set sum to the opposite of the carry bit, and reset carry bit
                sum.append(carry ? '0' : '1');
                carry = 0;
            }
        }

        // If there is a final carry bit, add it to the result
        if (carry == 1) {
            sum.append('1');
        }

        // Reverse the sum and remove leading zeroes
        std::reverse(sum.begin(), sum.end());
        while (sum[0] == '0' && sum.length() > 1) {
            sum.erase(0, 1);
        }

        // Write the result to the output port
        sum.write(sum);
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
