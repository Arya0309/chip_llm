
#include <systemc.h>


SC_MODULE(OctalToDecimalConverter) {
    // Input port for the octal number
    sc_in<int> octal_input;
    
    // Output port for the decimal equivalent
    sc_out<int> decimal_output;

    // Local variable to store the input octal number
    int num;

    // Local variable to store the decimal equivalent
    int dec_value;

    // Local variable to store the base value
    int base;

    // Function to convert octal to decimal
    void octalToDecimal() {
        // Update the input octal number
        num = octal_input.read();

        // Initialize the decimal equivalent
        dec_value = 0;

        // Initialize the base value
        base = 1;

        // Iterate through the octal number
        while (num != 0) {
            // Extract the last digit
            int last_digit = num % 10;

            // Divide the input octal number by 10
            int temp = num / 10;

            // Multiply the last digit by the base value
            dec_value += last_digit * base;

            // Update the base value
            base *= 8;
        }

        // Return the decimal equivalent
        decimal_output.write(dec_value);
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
