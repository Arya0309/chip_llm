
#include <systemc.h>
#include <vector>
#include <algorithm>

class BinaryNumber {
public:
    std::vector<bool> digits;

    BinaryNumber(const std::string& str) {
        for (char c : str) {
            digits.push_back(c == '1');
        }
    }

    BinaryNumber operator+(const BinaryNumber& other) const {
        BinaryNumber result;
        int maxSize = std::max(digits.size(), other.digits.size());
        bool carry = false;

        for (int i = 0; i < maxSize || carry; ++i) {
            bool bitA = i < digits.size() ? digits[i] : false;
            bool bitB = i < other.digits.size() ? other.digits[i] : false;
            bool sumBit = bitA ^ bitB ^ carry;
            carry = (bitA && bitB) || (bitA && carry) || (bitB && carry);
            result.digits.push_back(sumBit);
        }

        return result;
    }

    std::string toString() const {
        std::string str;
        for (bool digit : digits) {
            str += digit ? '1' : '0';
        }
        std::reverse(str.begin(), str.end());
        size_t pos = str.find_first_not_of('0');
        if (pos != std::string::npos) {
            return str.substr(pos);
        }
        return "0";
    }
};


SC_MODULE(BinaryAdder) {
    sc_in<std::string> A;  // Input port for the first binary string
    sc_in<std::string> B;  // Input port for the second binary string
    sc_out<std::string> sum; // Output port for the sum of the binary strings

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to perform binary addition
        SC_METHOD(addBinary);
        sensitive << A << B;
    }

    // Method to add the two binary strings
    void addBinary() {
        BinaryNumber numA(A.read());
        BinaryNumber numB(B.read());
        BinaryNumber result = numA + numB;
        sum.write(result.toString());
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
