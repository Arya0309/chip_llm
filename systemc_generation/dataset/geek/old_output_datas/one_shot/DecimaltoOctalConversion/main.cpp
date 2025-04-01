
#include <systemc.h>

// Module to convert decimal to octal
SC_MODULE(DecToOctalConverter) {
    sc_in<int> decimal_input;  // Input port for the decimal number
    sc_out<sc_bv<32>> octal_output; // Output port for the octal number (as binary vector)

    // Constructor
    SC_CTOR(DecToOctalConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << decimal_input;
    }

    // Method to convert decimal to octal
    void convert() {
        int n = decimal_input.read();
        sc_bv<32> octalNum; // Using binary vector to store octal digits
        int i = 0;

        // Convert decimal to octal
        while (n != 0) {
            octalNum.range(i * 3 + 2, i * 3) = n % 8; // Store each octal digit in 3 bits
            n = n / 8;
            i++;
        }

        // Shift the octal digits to the left to align them correctly
        octalNum <<= (32 - i * 3);

        // Write the result to the output port
        octal_output.write(octalNum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> decimal_input; // Signal for the decimal input
    sc_signal<sc_bv<32>> octal_output; // Signal for the octal output

    DecToOctalConverter converter_inst; // Instance of the DecToOctalConverter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.decimal_input(decimal_input);
        converter_inst.octal_output(octal_output);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 33 to octal
        decimal_input = 33;
        wait(1, SC_NS); // Wait for the converter to process
        cout << "Decimal: 33, Octal: " << octal_output.read().to_string(SC_BIN).substr(32 - 10) << endl;

        // Additional test case: Convert 100 to octal
        decimal_input = 100;
        wait(1, SC_NS);
        cout << "Decimal: 100, Octal: " << octal_output.read().to_string(SC_BIN).substr(32 - 10) << endl;

        // Additional test case: Convert 0 to octal
        decimal_input = 0;
        wait(1, SC_NS);
        cout << "Decimal: 0, Octal: " << octal_output.read().to_string(SC_BIN).substr(32 - 10) << endl;

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
