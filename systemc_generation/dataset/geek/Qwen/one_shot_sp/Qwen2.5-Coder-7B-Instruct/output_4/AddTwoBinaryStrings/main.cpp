
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the binary sum

    SC_CTOR(BinaryAdder) {
        SC_THREAD(add_binary);
        sensitive << A << B;
    }

    void add_binary() {
        std::string res;
        char carry = '0';

        // Adjust lengths
        int diff = B.length() - A.length();
        std::string padding(diff, '0');
        A = padding + A;

        // Loop through characters from right to left
        for (int i = A.length() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1')
                    res.push_back('1'), carry = '1';
                else
                    res.push_back('0'), carry = '1';
            } else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1')
                    res.push_back('1'), carry = '0';
                else
                    res.push_back('0'), carry = '0';
            } else if (A[i] != B[i]) {
                if (carry == '1')
                    res.push_back('0'), carry = '1';
                else
                    res.push_back('1'), carry = '0';
            }
        }

        // Handle final carry
        if (carry == '1')
            res.push_back(carry);

        // Reverse and trim leading zeros
        std::reverse(res.begin(), res.end());
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
