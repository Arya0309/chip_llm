C++
#include <systemc.h>

// Module that converts an octal input to its decimal representation

SC_MODULE(OctalToDecimalConverter) {
    // Input port for the octal input
    sc_in<int> octal_input;

    // Output port for the decimal output
    sc_out<int> decimal_output;

    // Variable to store the current base value
    int base = 1;

    // Function to convert an octal number to its decimal representation
    void octalToDecimal() {
        // Variable to store the current decimal value
        int decimal_value = 0;

        // Iterate through the octal representation of the input
        while (octal_input.read() != 0) {
            // Extract the last digit of the input
            int last_digit = octal_input.read() % 10;
            octal_input.read() = octal_input.read() / 10;

            // Calculate the decimal value by multiplying the last digit by the base and adding it to the accumulator
            decimal_value += last_digit * base;

            base = base * 8;
        }

        decimal_output.write(decimal_value);
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
