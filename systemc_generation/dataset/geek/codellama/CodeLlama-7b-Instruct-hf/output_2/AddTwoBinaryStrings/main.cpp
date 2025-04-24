
#include <systemc.h>

// Module that adds two binary strings

SC_MODULE(BinaryAdder) {
    sc_in<string> A; // Input port for the first binary string
    sc_in<string> B; // Input port for the second binary string
    sc_out<string> sum; // Output port for the result

    // Function to determine if the length of the two input
    // strings is the same
    void lengthCheck();

    // Function to add two binary strings with the length
    // the same or with the length of the longer string
    // as prefix
    void add();

    // Constructor
    SC_CTOR(BinaryAdder) {
        // Process to check the length of the two input strings
        SC_METHOD(lengthCheck);
        sensitive << A << B << sum;

        // Process to add the two binary strings with the
        // length the same or with the length of the longer string
        // as prefix
        SC_METHOD(add);
        sensitive << A << B << sum;
    }

    // Function to determine if the length of the two input
    // strings is the same
    void lengthCheck() {
        sc_int<32> lengthDiff = A.length() - B.length();
        sum.write(addBinary(A, B).substr(lengthDiff));
    }

    // Function to add two binary strings with the length
    // the same or with the length of the longer string
    // as prefix
    void add() {
        sum.write(addBinary(A, B));
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
