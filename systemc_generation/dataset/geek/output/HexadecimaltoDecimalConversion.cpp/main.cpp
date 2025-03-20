
#include <systemc.h>
#include <string>

// Module to convert hexadecimal to decimal
SC_MODULE(HexToDecConverter) {
    // Input port for hexadecimal string
    sc_in<std::string> hex_input;
    
    // Output port for decimal value
    sc_out<int> dec_output;

    // Internal signal to store the decimal value
    sc_signal<int> dec_value;

    // Process to perform hexadecimal to decimal conversion
    void convertHexToDec() {
        std::string hexVal = hex_input.read();
        int len = hexVal.size();
        int base = 1;
        int dec_val = 0;

        // Conversion logic similar to the C++ code
        for (int i = len - 1; i >= 0; i--) {
            if (hexVal[i] >= '0' && hexVal[i] <= '9') {
                dec_val += (int(hexVal[i]) - 48) * base;
                base = base * 16;
            } else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') {
                dec_val += (int(hexVal[i]) - 55) * base;
                base = base * 16;
            }
        }

        // Write the result to the output port
        dec_output.write(dec_val);
    }

    // Constructor to register the process
    SC_CTOR(HexToDecConverter) {
        SC_METHOD(convertHexToDec);
        sensitive << hex_input;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with HexToDecConverter
    sc_signal<std::string> hex_input;
    sc_signal<int> dec_output;

    // Instance of HexToDecConverter
    HexToDecConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the hexadecimal input
        hex_input = "1A";

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        cout << "Hexadecimal input: " << hex_input.read() << endl;
        cout << "Decimal output: " << dec_output.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.hex_input(hex_input);
        converter.dec_output(dec_output);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
