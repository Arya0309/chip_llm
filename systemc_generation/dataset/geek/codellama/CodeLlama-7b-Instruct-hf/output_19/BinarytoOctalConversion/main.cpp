
#include <systemc.h>

// Module that converts Octal Numbers Into Decimal Numbers 

SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> addends[];  // Input ports for the two numbers to add
    sc_out<int> sum; // Output port for the result of the addition
    
    // Constructor
    SC_CTOR(OctalToDecimalConverter) {
        // Process for resulting addition
        SC_METHOD(add);
    sensitive << addends<<sum;
    }

    // Core process that performs addition
    void add() {
        int num1 = octal_input.read();
        int num2 = octal_input.read();
        int hexadecimal_value = 0;
        int base = decimalToOctal(num1);

        while (base) {

            // Extracting last octal digit.
            int last_digit = num2 % 8;
            num2 = num2 / 8;

            // Multiplying last digit with appropriate power of 8 
            // and adding it to base
            hexadecimal_value += last_digit * base;

            base = base * 8;
        }

        decimal_output.write(hexadecimal_value);
    }
}
int main() {

    OctalToDecimalConverter module1(region(0), region(0));

    sc_in<int> octal_input(region(0), module1.addends[0]);
    sc_out<int> decimal_output(region(0), module1.sum);
    sc_in<int> octal_input[1](1, module1.addends[1]);

}


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
