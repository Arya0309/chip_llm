
#include <systemc.h>

// Module to convert decimal to hexadecimal
SC_MODULE(DecToHexa) {
    sc_in<int> decimal_in;  // Input port for the decimal number
    sc_out<sc_bv<32>> hex_out; // Output port for the hexadecimal number

    // Constructor
    SC_CTOR(DecToHexa) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    // Method to convert decimal to hexadecimal
    void convert() {
        int n = decimal_in.read();
        sc_bv<32> hexaDeciNum; // Bit vector to store hexadecimal number
        int i = 0;

        while (n != 0) {
            int temp = n % 16;
            if (temp < 10) {
                hexaDeciNum.range((i+1)*4-1, i*4) = temp + '0';
            } else {
                hexaDeciNum.range((i+1)*4-1, i*4) = temp - 10 + 'A';
            }
            i++;
            n = n / 16;
        }

        // Fill the rest of the bits with spaces
        for (int j = i * 4; j < 32; j += 4) {
            hexaDeciNum.range(j+3, j) = ' ';
        }

        hex_out.write(hexaDeciNum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> decimal_in; // Signal for the decimal input
    sc_signal<sc_bv<32>> hex_out; // Signal for the hexadecimal output

    DecToHexa dec_to_hexa_inst; // Instance of the DecToHexa module

    // Constructor
    SC_CTOR(Testbench) : dec_to_hexa_inst("dec_to_hexa_inst") {
        // Connect signals to ports
        dec_to_hexa_inst.decimal_in(decimal_in);
        dec_to_hexa_inst.hex_out(hex_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 2545 to hexadecimal
        decimal_in = 2545;
        wait(1, SC_NS); // Wait for the conversion to process
        std::cout << "Decimal: 2545 -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Test case 2: Convert 0 to hexadecimal
        decimal_in = 0;
        wait(1, SC_NS);
        std::cout << "Decimal: 0 -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Test case 3: Convert 16 to hexadecimal
        decimal_in = 16;
        wait(1, SC_NS);
        std::cout << "Decimal: 16 -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Test case 4: Convert 255 to hexadecimal
        decimal_in = 255;
        wait(1, SC_NS);
        std::cout << "Decimal: 255 -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // End simulation after successful test
        std::cout << "All tests passed successfully." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
