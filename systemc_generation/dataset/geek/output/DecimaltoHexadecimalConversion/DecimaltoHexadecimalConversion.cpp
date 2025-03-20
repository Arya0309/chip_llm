
#include <systemc.h>

// Module to convert decimal to hexadecimal
SC_MODULE(DecimalToHex) {
    sc_in<int> decimal_in;  // Input port for decimal number
    sc_out<sc_bv<32>> hex_out;  // Output port for hexadecimal number (up to 32 bits)

    // Internal storage for hexadecimal digits
    sc_bv<32> hexaDeciNum;
    int i;

    // Process to perform the conversion
    void convertToHex() {
        int n = decimal_in.read();
        i = 0;
        hexaDeciNum = 0;  // Initialize the hexadecimal number to zero

        while (n != 0) {
            int temp = n % 16;
            if (temp < 10) {
                hexaDeciNum.range(i * 4 + 3, i * 4) = temp + 48;  // Store ASCII value of '0' to '9'
            } else {
                hexaDeciNum.range(i * 4 + 3, i * 4) = temp + 55;  // Store ASCII value of 'A' to 'F'
            }
            i++;
            n = n / 16;
        }

        // Reverse the hexadecimal digits
        sc_bv<32> reversedHex;
        for (int j = 0; j < i; j++) {
            reversedHex.range((i - j - 1) * 4 + 3, (i - j - 1) * 4) = hexaDeciNum.range(j * 4 + 3, j * 4);
        }

        // Output the reversed hexadecimal number
        hex_out.write(reversedHex);
    }

    // Constructor to register the process
    SC_CTOR(DecimalToHex) {
        SC_METHOD(convertToHex);
        sensitive << decimal_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    sc_signal<int> decimal_in;  // Signal to connect with DecimalToHex
    sc_signal<sc_bv<32>> hex_out;

    // Instance of DecimalToHex
    DecimalToHex converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing the decimal number
        decimal_in = 2545;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        sc_bv<32> hexResult = hex_out.read();
        std::cout << "Decimal: " << decimal_in.read() << " -> Hexadecimal: ";
        for (int i = 0; i < 8; i++) {  // Assuming up to 8 characters for the hex representation
            char c = hexResult.range(i * 4 + 3, i * 4).to_uint();
            if (c != 0)  // Only print non-zero characters
                std::cout << c;
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
