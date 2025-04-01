
#include <systemc.h>

// Module to convert hexadecimal to decimal
SC_MODULE(HexToDecConverter) {
    sc_in<std::string> hex_in;  // Input port for hexadecimal string
    sc_out<int> dec_out;        // Output port for decimal value

    // Constructor
    SC_CTOR(HexToDecConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << hex_in;
    }

    // Method to convert hexadecimal to decimal
    void convert() {
        std::string hexVal = hex_in.read();
        int len = hexVal.size();
        int base = 1;
        int dec_val = 0;

        for (int i = len - 1; i >= 0; i--) {
            if (hexVal[i] >= '0' && hexVal[i] <= '9') {
                dec_val += (int(hexVal[i]) - 48) * base;
                base = base * 16;
            } else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
                dec_val += (int(hexVal[i]) - 55) * base;
                base = base * 16;
            }
        }
        dec_out.write(dec_val);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> hex_sig; // Signal for the hexadecimal input
    sc_signal<int> dec_sig;         // Signal for the decimal output

    // Instance of the HexToDecConverter module
    HexToDecConverter converter_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : converter_inst("converter_inst") {
        // Connect signals to module ports
        converter_inst.hex_in(hex_sig);
        converter_inst.dec_out(dec_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case: hex = "1A" (expected decimal = 26)
        hex_sig.write("1A");
        wait(1, SC_NS); // Wait for the process to update
        int result = dec_sig.read();
        std::cout << "Hexadecimal: 1A -> Decimal: " << result << std::endl;
        assert(result == 26);

        // Additional test case: hex = "FF" (expected decimal = 255)
        hex_sig.write("FF");
        wait(1, SC_NS);
        result = dec_sig.read();
        std::cout << "Hexadecimal: FF -> Decimal: " << result << std::endl;
        assert(result == 255);

        // Additional test case: hex = "0" (expected decimal = 0)
        hex_sig.write("0");
        wait(1, SC_NS);
        result = dec_sig.read();
        std::cout << "Hexadecimal: 0 -> Decimal: " << result << std::endl;
        assert(result == 0);

        // Additional test case: hex = "10" (expected decimal = 16)
        hex_sig.write("10");
        wait(1, SC_NS);
        result = dec_sig.read();
        std::cout << "Hexadecimal: 10 -> Decimal: " << result << std::endl;
        assert(result == 16);

        // End simulation after successful tests
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
