
#include <systemc.h>

// Module to convert decimal to hexadecimal
SC_MODULE(DecimalToHex) {
    sc_in<int> decimal_in;  // Input port for the decimal number
    sc_out<sc_bv<32>> hex_out; // Output port for the hexadecimal representation

    // Constructor
    SC_CTOR(DecimalToHex) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    // Method to convert decimal to hexadecimal
    void convert() {
        int n = decimal_in.read();
        char hexaDeciNum[100];
        int i = 0;

        while (n != 0) {
            int temp = n % 16;
            if (temp < 10) {
                hexaDeciNum[i] = temp + 48;
                i++;
            } else {
                hexaDeciNum[i] = temp + 55;
                i++;
            }
            n = n / 16;
        }

        // Constructing the hexadecimal string
        std::string hexString = "";
        for (int j = i - 1; j >= 0; j--) {
            hexString += hexaDeciNum[j];
        }

        // Converting the string to a binary vector
        sc_bv<32> hexBv(hexString.c_str());

        // Writing the result to the output port
        hex_out.write(hexBv);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> decimal_in; // Signal for the decimal number
    sc_signal<sc_bv<32>> hex_out; // Signal for the hexadecimal representation

    DecimalToHex converter_inst; // Instance of the DecimalToHex module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.decimal_in(decimal_in);
        converter_inst.hex_out(hex_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Simple conversion
        decimal_in = 2545;
        wait(1, SC_NS); // Wait for the converter to process
        std::cout << "Decimal: " << decimal_in.read() << " -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Test case 2: Another conversion
        decimal_in = 10;
        wait(1, SC_NS);
        std::cout << "Decimal: " << decimal_in.read() << " -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Test case 3: Large number
        decimal_in = 123456789;
        wait(1, SC_NS);
        std::cout << "Decimal: " << decimal_in.read() << " -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
