
#include <systemc.h>

// Module that converts octal to decimal

SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_input; // Input port for the octal number
    sc_out<int> decimal_output; // Output port for the decimal equivalent

    // Constructor
    SC_CTOR(OctalToDecimalConverter) {
        // Process to convert octal to decimal
        SC_METHOD(convert);
        sensitive << octal_input;
    }

    // Method to convert octal to decimal
    void convert() {
        // Initialize base value to 1
        int base = 1;

        // Declare temporary variable to iterate over octal digits
        int n = octal_input.read();

        // Iterate over octal digits using while loop
        while (n > 0) {
            // Extract last digit of n
            int last_digit = n % 10;

            // Multiply last digit by appropriate power of 8
            base = base * 8;

            // Add product to decimal value
            decimal_output.write(decimal_output.read() + last_digit * base);

            // Update n and base
            n = n / 10;
            base = base * 8;
        }
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
