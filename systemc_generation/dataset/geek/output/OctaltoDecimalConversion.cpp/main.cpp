
#include <systemc.h>

// Module to convert octal to decimal
SC_MODULE(OctalToDecimalConverter) {
    // Input port for the octal number
    sc_in<int> octal_input;
    
    // Output port for the decimal number
    sc_out<int> decimal_output;

    // Process to perform the conversion
    void convertOctalToDecimal() {
        // Read the input octal number
        int num = octal_input.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        // Conversion logic
        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        // Write the resulting decimal number
        decimal_output.write(dec_value);
    }

    // Constructor to register the process
    SC_CTOR(OctalToDecimalConverter) {
        SC_METHOD(convertOctalToDecimal);
        sensitive << octal_input;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with OctalToDecimalConverter
    sc_signal<int> octal_input;
    sc_signal<int> decimal_output;

    // Instance of OctalToDecimalConverter
    OctalToDecimalConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the octal number
        octal_input = 67;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        cout << "Octal number: " << octal_input.read() << endl;
        cout << "Decimal equivalent: " << decimal_output.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.octal_input(octal_input);
        converter.decimal_output(decimal_output);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
