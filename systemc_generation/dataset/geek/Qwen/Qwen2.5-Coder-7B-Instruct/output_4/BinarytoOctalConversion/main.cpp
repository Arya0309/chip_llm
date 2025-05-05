
#include <systemc.h>


SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_input;  // Input port for the octal number
    sc_out<int> decimal_output; // Output port for the decimal number

    SC_CTOR(OctalToDecimalConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << octal_input;
    }

    void convert() {
        int num = octal_input.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

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
