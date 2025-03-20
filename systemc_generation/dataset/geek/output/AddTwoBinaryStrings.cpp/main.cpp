
#include <systemc.h>
#include <string>
#include <algorithm>

// SystemC Module for Binary Addition
SC_MODULE(BinaryAdder) {
    // Input ports for binary strings
    sc_in<std::string> bin1, bin2;
    
    // Output port for the resulting binary string
    sc_out<std::string> result;

    // Internal method to perform binary addition
    void addBinaryStrings() {
        std::string A = bin1.read();
        std::string B = bin2.read();
        
        // Ensure A is not longer than B
        if (A.length() > B.length()) {
            std::swap(A, B);
        }

        // Calculate the difference in length
        int diff = B.length() - A.length();
        std::string padding(diff, '0');
        A = padding + A;

        std::string res;
        char carry = '0';

        // Perform binary addition
        for (int i = A.length() - 1; i >= 0; i--) {
            if (A[i] == '1' && B[i] == '1') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '1';
                } else {
                    res.push_back('0');
                    carry = '1';
                }
            } else if (A[i] == '0' && B[i] == '0') {
                if (carry == '1') {
                    res.push_back('1');
                    carry = '0';
                } else {
                    res.push_back('0');
                    carry = '0';
                }
            } else if (A[i] != B[i]) {
                if (carry == '1') {
                    res.push_back('0');
                    carry = '1';
                } else {
                    res.push_back('1');
                    carry = '0';
                }
            }
        }

        // Handle final carry
        if (carry == '1') {
            res.push_back(carry);
        }

        // Reverse the result
        std::reverse(res.begin(), res.end());

        // Remove leading zeros
        size_t index = 0;
        while (index + 1 < res.length() && res[index] == '0') {
            index++;
        }
        res = res.substr(index);

        // Write the result to the output port
        result.write(res);
    }

    // Constructor to register the process
    SC_CTOR(BinaryAdder) {
        SC_METHOD(addBinaryStrings);
        sensitive << bin1 << bin2;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with BinaryAdder
    sc_signal<std::string> bin1, bin2;
    sc_signal<std::string> sum;

    // Instance of BinaryAdder
    BinaryAdder adder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize binary strings
        bin1 = "1101";
        bin2 = "100";

        // Wait for one delta cycle to let the adder process
        wait(1, SC_NS);

        // Print the results
        std::cout << "Binary number 1: " << bin1.read() << std::endl;
        std::cout << "Binary number 2: " << bin2.read() << std::endl;
        std::cout << "Sum of binary numbers: " << sum.read() << std::endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : adder("adder") {
        // Connecting signals to the adder ports
        adder.bin1(bin1); adder.bin2(bin2);
        adder.result(sum);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
