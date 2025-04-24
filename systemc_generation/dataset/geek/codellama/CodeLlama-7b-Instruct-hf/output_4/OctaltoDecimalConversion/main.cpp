scss
#include <systemc.h>

// Function to convert octal to decimal
void octalToDecimal(int n) 
{
    int num = n;
    int dec_value = 0;
    int base = 1;

    // While loop to iterate over the digits of n
    while (num) {
        // Extract the last digit of n
        int last_digit = num % 10;
        num = num / 10;

        // Multiply last_digit by the appropriate base and add it to dec_value
        dec_value += last_digit * base;
        base *= 8;
    }

    // Return the decimal equivalent of n
    return dec_value;
}

// Module that computes the decimal equivalent of an octal number

SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_input;  // Input port for the octal number
    sc_out<int> decimal_output; // Output port for the decimal equivalent

    // Function to convert octal to decimal
    void convertOctalToDecimalConverter() {
        // Call the octalToDecimal function to compute the decimal equivalent
        int dec_value = octalToDecimal(octal_input.read());

        // Write the result to the decimal_output port
        decimal_output.write(dec_value);
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
