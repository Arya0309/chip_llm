
#include <systemc.h>

// SystemC Module for Octal to Decimal Conversion
SC_MODULE(OctalToDecimalConverter) {
    // Input port for octal number
    sc_in<int> octal_in;
    
    // Output port for decimal number
    sc_out<int> decimal_out;

    // Process to perform the conversion
    void convert() {
        int num = octal_in.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        decimal_out.write(dec_value);
    }

    // Constructor to register the process
    SC_CTOR(OctalToDecimalConverter) {
        SC_METHOD(convert);
        sensitive << octal_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with OctalToDecimalConverter
    sc_signal<int> octal_num;
    sc_signal<int> decimal_result;

    // Instance of OctalToDecimalConverter
    OctalToDecimalConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize octal number
        octal_num = 67;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        cout << "Octal number: " << octal_num.read() << endl;
        cout << "Decimal equivalent: " << decimal_result.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.octal_in(octal_num);
        converter.decimal_out(decimal_result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
