
#include <systemc.h>

// Module to convert hexadecimal to decimal
SC_MODULE(HexToDecConverter) {
    sc_in<std::string> hex_input;  // Input port for the hexadecimal string
    sc_out<int> dec_output;         // Output port for the decimal value

    // Constructor
    SC_CTOR(HexToDecConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << hex_input;
    }

    // Method to convert hexadecimal to decimal
    void convert() {
        std::string hexVal = hex_input.read();
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
        dec_output.write(dec_val);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<std::string> hex_input; // Signal for the hexadecimal input
    sc_signal<int> dec_output;         // Signal for the decimal output

    HexToDecConverter converter_inst; // Instance of the HexToDecConverter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.hex_input(hex_input);
        converter_inst.dec_output(dec_output);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Simple conversion
        hex_input = "1A";
        wait(1, SC_NS); // Wait for the converter to process
        assert(dec_output.read() == 26);
        cout << "Hex: 1A -> Dec: " << dec_output.read() << endl;

        // Test case 2: Another simple conversion
        hex_input = "FF";
        wait(1, SC_NS);
        assert(dec_output.read() == 255);
        cout << "Hex: FF -> Dec: " << dec_output.read() << endl;

        // Test case 3: Zero conversion
        hex_input = "0";
        wait(1, SC_NS);
        assert(dec_output.read() == 0);
        cout << "Hex: 0 -> Dec: " << dec_output.read() << endl;

        // Test case 4: Large number conversion
        hex_input = "123ABC";
        wait(1, SC_NS);
        assert(dec_output.read() == 1194684);
        cout << "Hex: 123ABC -> Dec: " << dec_output.read() << endl;

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
