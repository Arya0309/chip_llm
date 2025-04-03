
#include <systemc.h>

// Module that adds two binary strings
SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum binary string

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(add);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void add() {
        std::string a = A.read();
        std::string b = B.read();

        // Ensure A is the shorter or equal-length string
        if (a.length() > b.length())
            return addBinary(b, a);

        // Calculate the difference in length
        int diff = b.length() - a.length();
        std::string padding;
        for (int i = 0; i < diff; i++)
            padding.push_back('0');

        // Pad the shorter string with leading zeros
        a = padding + a;

        std::string res;
        char carry = '0';

        // Perform binary addition
        for (int i = a.length() - 1; i >= 0; i--) {
            if (a[i] == '1' && b[i] == '1') {
                if (carry == '1')
                    res.push_back('1'), carry = '1';
                else
                    res.push_back('0'), carry = '1';
            } else if (a[i] == '0' && b[i] == '0') {
                if (carry == '1')
                    res.push_back('1'), carry = '0';
                else
                    res.push_back('0'), carry = '0';
            } else if (a[i] != b[i]) {
                if (carry == '1')
                    res.push_back('0'), carry = '1';
                else
                    res.push_back('1'), carry = '0';
            }
        }

        // Handle final carry
        if (carry == '1')
            res.push_back(carry);

        // Reverse the result and remove leading zeros
        std::reverse(res.begin(), res.end());
        int index = 0;
        while (index + 1 < res.length() && res[index] == '0')
            index++;

        sum.write(res.substr(index));
    }

    // Helper function to handle recursive call
    std::string addBinary(const std::string& A, const std::string& B) {
        return BinaryAdder::addBinary(A, B);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> a; // Signal for the first binary string
    sc_signal<std::string> b; // Signal for the second binary string
    sc_signal<std::string> sum; // Signal for the sum binary string

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
        cout << "Test case 1: " << sum.read() << endl;

        // Test case 2: Equal length strings
        a = "1111"; b = "0001";
        wait(1, SC_NS);
        assert(sum.read() == "10000");
        cout << "Test case 2: " << sum.read() << endl;

        // Test case 3: Different lengths with carry
        a = "1"; b = "111";
        wait(1, SC_NS);
        assert(sum.read() == "1000");
        cout << "Test case 3: " << sum.read() << endl;

        // Test case 4: All zeros
        a = "0"; b = "0";
        wait(1, SC_NS);
        assert(sum.read() == "0");
        cout << "Test case 4: " << sum.read() << endl;

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
