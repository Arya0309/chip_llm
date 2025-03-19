
#include <systemc.h>

// Module to convert binary to decimal
SC_MODULE(BinaryToDecimalConverter) {
    // Input port for binary number
    sc_in<int> binary_in;
    
    // Output port for decimal number
    sc_out<int> decimal_out;

    // Process to perform conversion
    void convertBinaryToDecimal() {
        int num = binary_in.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 2;
        }

        decimal_out.write(dec_value);
    }

    // Constructor to register the process
    SC_CTOR(BinaryToDecimalConverter) {
        SC_METHOD(convertBinaryToDecimal);
        sensitive << binary_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with BinaryToDecimalConverter
    sc_signal<int> binary_input;
    sc_signal<int> decimal_output;

    // Instance of BinaryToDecimalConverter
    BinaryToDecimalConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize binary number
        binary_input = 10101001;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the result
        cout << "Binary: " << binary_input.read() << " -> Decimal: " << decimal_output.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.binary_in(binary_input);
        converter.decimal_out(decimal_output);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
