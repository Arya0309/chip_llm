scss
#include <systemc.h>

// Module to convert octal to decimal

SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_input; // Input port for octal number
    sc_out<int> decimal_output; // Output port for decimal value

    // Initializing base value to 1
    int base = 1;

    // Function to convert octal to decimal
    void octalToDecimal() {
        int num = octal_input.read(); // Read input octal number
        int dec_value = 0; // Initialize decimal value to 0

        while (num > 0) {
            // Extract last digit of octal number
            int last_digit = num % 10;
            num = num / 10;

            // Multiply last digit by appropriate base value
            dec_value += last_digit * base;

            base = base * 8;
        }

        decimal_output.write(dec_value); // Output decimal value
    }

    // Constructor
    SC_CTOR(OctalToDecimalConverter) {
        // Call octalToDecimal function when octal input changes
        sc_method(octalToDecimal);
        sensitive << octal_input;
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> octal_input_sig; // Signal for the octal input
    sc_signal<int> decimal_output_sig; // Signal for the decimal output

    OctalToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.octal_input(octal_input_sig);
        converter_inst.decimal_output(decimal_output_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 67 (octal) to decimal
        octal_input_sig = 67;
        wait(1, SC_NS); // Wait for the converter to process
        int result = decimal_output_sig.read();
        cout << "Octal: 67 -> Decimal: " << result << endl;
        assert(result == 55); // 67 in octal is 55 in decimal

        // Additional test case: Convert 123 (octal) to decimal
        octal_input_sig = 123;
        wait(1, SC_NS);
        result = decimal_output_sig.read();
        cout << "Octal: 123 -> Decimal: " << result << endl;
        assert(result == 83); // 123 in octal is 83 in decimal

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
