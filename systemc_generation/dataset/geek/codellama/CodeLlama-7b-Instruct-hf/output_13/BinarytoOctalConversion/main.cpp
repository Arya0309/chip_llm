
#include <systemc.h>

// Module that converts octal to decimal

SC_MODULE(OctalToDecimalConverter) {
    // Ports
    sc_in<int> octal_input;  // Input port for the octal value
    sc_out<int> decimal_output;  // Output port for the decimal value

    // Constructor
    SC_CTOR(OctalToDecimalConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << octal_input;
    }

    // Method to perform conversion
    void convert() {
        // Calculate the decimal value
        int dec_value = 0;
        int num = octal_input.read();
        int base = 1;
        while (num) {
            int last_digit = num % 10;
            num = num / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        // Output the decimal value
        decimal_output.write(dec_value);
    }
};

// Testbench

SC_MODULE(OctalToDecimalConverterTestbench) {
    // Ports
    sc_clock clk;
    sc_signal<int> octal_input;
    sc_signal<int> decimal_output;

    // Instantiate the OctalToDecimalConverter module
    OctalToDecimalConverter octal_to_decimal;

    // Constructor
    SC_CTOR(OctalToDecimalConverterTestbench) : clk("clk", 10, SC_NS) {
        octal_to_decimal.octal_input(octal_input);  // Connect octal_input to the module's input port
        octal_to_decimal.decimal_output(decimal_output);  // Connect decimal_output to the module's output port
    }

    void stimulus() {
        // Generate octal input values and corresponding decimal outputs
        octal_input.write(67);
        wait(10);
        octal_input.write(101);
        wait(10);
        octal_input.write(17);
        wait(10);
        octal_input.write(77);
        wait(10);

        // Read back the computed decimal outputs
        int decimal_value = decimal_output.read();
        cout << "Decimal value: " << decimal_value << endl;
    }

    SC_CTOR : OctalToDecimalConverterTestbench(OctalToDecimalConverterTestbench) {
        clk.start(SC_ZERO_TIME);
        sc_thread(stimulus);
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> octal_input; // Signal for the octal number
    sc_signal<int> decimal_output; // Signal for the decimal number

    OctalToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.octal_input(octal_input);
        converter_inst.decimal_output(decimal_output);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Octal 67
        octal_input = 67;
        wait(1, SC_NS); // Wait for the converter to process
        assert(decimal_output.read() == 55);
        cout << "Octal 67 -> Decimal " << decimal_output.read() << endl;

        // Test case 2: Octal 123
        octal_input = 123;
        wait(1, SC_NS);
        assert(decimal_output.read() == 83);
        cout << "Octal 123 -> Decimal " << decimal_output.read() << endl;

        // Test case 3: Octal 0
        octal_input = 0;
        wait(1, SC_NS);
        assert(decimal_output.read() == 0);
        cout << "Octal 0 -> Decimal " << decimal_output.read() << endl;

        // Test case 4: Octal 10
        octal_input = 10;
        wait(1, SC_NS);
        assert(decimal_output.read() == 8);
        cout << "Octal 10 -> Decimal " << decimal_output.read() << endl;

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
