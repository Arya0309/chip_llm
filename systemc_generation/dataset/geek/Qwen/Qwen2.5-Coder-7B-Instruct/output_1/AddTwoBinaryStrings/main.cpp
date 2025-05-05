
#include <systemc.h>


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum of the two binary strings

    SC_CTOR(BinaryAdder) {
        SC_THREAD(add);
        sensitive << A << B;
    }

    void add() {
        std::string a = A.read();
        std::string b = B.read();
        
        // Ensure a is the shorter string
        if (a.length() > b.length()) {
            std::swap(a, b);
        }
        
        int diff = b.length() - a.length();
        std::string padding(diff, '0');
        a = padding + a;
        
        std::string res(a.length(), '0');
        char carry = '0';
        
        for (int i = a.length() - 1; i >= 0; i--) {
            int bit_a = a[i] - '0';
            int bit_b = b[i] - '0';
            
            if (bit_a == 1 && bit_b == 1) {
                if (carry == '1') {
                    res[i] = '1';
                } else {
                    res[i] = '0';
                }
                carry = '1';
            } else if (bit_a == 0 && bit_b == 0) {
                if (carry == '1') {
                    res[i] = '1';
                    carry = '0';
                } else {
                    res[i] = '0';
                }
            } else {  // bit_a != bit_b
                if (carry == '1') {
                    res[i] = '0';
                } else {
                    res[i] = '1';
                }
            }
        }
        
        if (carry == '1') {
            res = '1' + res;
        }
        
        // Remove leading zeros
        size_t first_non_zero = res.find_first_not_of('0');
        if (first_non_zero != std::string::npos) {
            res = res.substr(first_non_zero);
        } else {
            res = "0";  // If all characters were zero, the result should be "0"
        }
        
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
