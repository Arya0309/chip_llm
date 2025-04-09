
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum binary string

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(addBinary);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void addBinary() {
        std::string a = A.read();
        std::string b = B.read();

        // If the length of string A is greater than the length of B then just swap
        if (a.length() > b.length())
            std::swap(a, b);

        // Calculating the difference between the length of the two strings.
        int diff = b.length() - a.length();

        // Initialise the padding string which is used to store zeroes that should
        // be added as prefix to the string which has length smaller than the other string.
        std::string padding;
        for (int i = 0; i < diff; i++)
            padding.push_back('0');

        a = padding + a;
        std::string res;
        char carry = '0';

        for (int i = a.length() - 1; i >= 0; i--) {
            // This if condition solves 110 111 possible cases
            if (a[i] == '1' && b[i] == '1') {
                if (carry == '1')
                    res.push_back('1'), carry = '1';
                else
                    res.push_back('0'), carry = '1';
            }

            // This if condition solves 000 001 possible cases
            else if (a[i] == '0' && b[i] == '0') {
                if (carry == '1')
                    res.push_back('1'), carry = '0';
                else
                    res.push_back('0'), carry = '0';
            }

            // This if condition solves 100 101 010 011 possible cases
            else if (a[i] != b[i]) {
                if (carry == '1')
                    res.push_back('0'), carry = '1';
                else
                    res.push_back('1'), carry = '0';
            }
        }

        // If at the end their is carry then just add it to the result
        if (carry == '1')
            res.push_back(carry);
        // reverse the result
        std::reverse(res.begin(), res.end());

        // To remove leading zeroes
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0')
            index++;
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
