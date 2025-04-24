c
// C++ program to convert octal to decimal using SystemC
#include <systemc.h>


SC_MODULE(OctalToDecimalConverter) {
    // Input and output ports
    sc_in<int> octal_input;
    sc_out<int> decimal_output;

    // Constructor
    SC_CTOR(OctalToDecimalConverter) {
        // Declare a register to store the base value
        SC_MEMBER(int) base = 1;

        // Declare a register to store the current digit
        SC_MEMBER(int) digit = 0;

        // Declare a register to store the decimal value
        SC_MEMBER(int) dec = 0;

        // Process to perform the conversion
        SC_METHOD(convert);
        sensitive << octal_input;
    }

    // Method to perform the conversion
    void convert() {
        // Initialize the base value
        base = 1;

        // Extract the last digit of the input number
        digit = octal_input.read() % 10;

        // Multiply the last digit by the base value and add it to the decimal value
        dec = 0;
        while (digit > 0) {
            dec += (digit * base);
            base *= 8;
            digit = (digit / 10) % 10;
        }

        // Calculate the decimal equivalent of the input number
        decimal_output.write(dec);
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
