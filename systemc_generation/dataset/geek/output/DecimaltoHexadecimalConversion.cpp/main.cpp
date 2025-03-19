
#include <systemc.h>
#include <iostream>

// Module to convert decimal to hexadecimal
SC_MODULE(DecimalToHexadecimal) {
    // Input port for the decimal number
    sc_in<int> decimal_in;
    
    // Output port for the hexadecimal string
    sc_out<sc_bv<100>> hex_out;

    // Internal storage for the hexadecimal string
    sc_bv<100> hex_string;

    // Process to perform the conversion
    void convert() {
        int n = decimal_in.read();
        int i = 0;

        // Conversion logic similar to the C++ code
        while (n != 0) {
            int temp = n % 16;
            if (temp < 10) {
                hex_string.range(i*8+7, i*8) = temp + 48; // Store ASCII value for '0'-'9'
            } else {
                hex_string.range(i*8+7, i*8) = temp + 55; // Store ASCII value for 'A'-'F'
            }
            i++;
            n = n / 16;
        }

        // Reverse the string to get the correct order
        sc_bv<100> reversed_hex;
        for (int j = 0; j < i; j++) {
            reversed_hex.range(j*8+7, j*8) = hex_string.range((i-j-1)*8+7, (i-j-1)*8);
        }

        // Write the result to the output port
        hex_out.write(reversed_hex);
    }

    // Constructor to register the process
    SC_CTOR(DecimalToHexadecimal) {
        SC_METHOD(convert);
        sensitive << decimal_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with DecimalToHexadecimal
    sc_signal<int> decimal_in;
    sc_signal<sc_bv<100>> hex_out;

    // Instance of DecimalToHexadecimal
    DecimalToHexadecimal converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the decimal number
        decimal_in = 2545;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        sc_bv<100> hex_result = hex_out.read();
        std::cout << "Decimal: 2545 -> Hexadecimal: ";
        bool start_printing = false;
        for (int i = 0; i < 100; i += 8) {
            char c = hex_result.range(i+7, i).to_uint();
            if (c != 0 || start_printing) {
                std::cout << c;
                start_printing = true;
            }
        }
        std::cout << std::endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.decimal_in(decimal_in);
        converter.hex_out(hex_out);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
