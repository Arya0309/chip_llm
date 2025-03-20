
#include <systemc.h>

// Module to convert a decimal number to binary
SC_MODULE(DecToBinConverter) {
    sc_in<int> n;          // Input port for the decimal number
    sc_out<sc_bv<32>> bin;  // Output port for the binary number (32 bits)

    // Constructor
    SC_CTOR(DecToBinConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << n;
    }

    // Method to convert decimal to binary
    void convert() {
        int num = n.read();
        sc_bv<32> binaryNum = 0; // Initialize binary number to 0

        // Conversion logic
        int i = 0;
        while (num > 0) {
            binaryNum[i] = num % 2;
            num = num / 2;
            i++;
        }

        // Assign the binary number to the output port
        bin.write(binaryNum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> n_sig;           // Signal for the input decimal number
    sc_signal<sc_bv<32>> bin_sig;   // Signal for the output binary number

    DecToBinConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.n(n_sig);
        converter_inst.bin(bin_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 10 to binary
        n_sig = 10;
        wait(1, SC_NS); // Wait for the converter to process
        sc_bv<32> result = bin_sig.read();
        std::cout << "Decimal: 10 -> Binary: " << result << std::endl;

        // Additional test case: Convert 0 to binary
        n_sig = 0;
        wait(1, SC_NS);
        result = bin_sig.read();
        std::cout << "Decimal: 0 -> Binary: " << result << std::endl;

        // Additional test case: Convert 255 to binary
        n_sig = 255;
        wait(1, SC_NS);
        result = bin_sig.read();
        std::cout << "Decimal: 255 -> Binary: " << result << std::endl;

        // End simulation after successful test
        std::cout << "Tests completed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
