scss
#include <systemc.h>

// Module that converts binary to decimal

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary_in;  // Binary input signal
    sc_out<int> decimal_out;  // Decimal output signal

    // Function to convert binary to decimal
    void binaryToDecimal() {
        int num = binary_in.read();  // Read the binary number from the input signal
        int dec_value = 0;  // Initialize the decimal value to 0
        int base = 1;  // Initialize the base value to 1

        while (num) {  // Iterate through the binary number digit by digit
            int last_digit = num % 10;  // Extract the last digit of the binary number
            num = num / 10;  // Move to the next digit
            dec_value += last_digit * base;  // Multiply the last digit by the base value and add it to the decimal value
            base = base * 2;  // Multiply the base value by 2
        }

        decimal_out.write(dec_value);  // Write the decimal value to the output signal
    }

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        // Process to perform the conversion
        SC_METHOD(binaryToDecimal);
        sensitive << binary_in;
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> binary_sig; // Signal for the binary number
    sc_signal<int> decimal_sig; // Signal for the decimal number

    BinaryToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.binary_in(binary_sig);
        converter_inst.decimal_out(decimal_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert binary 10101001 to decimal
        binary_sig = 10101001;
        wait(1, SC_NS); // Wait for the conversion to process
        int result = decimal_sig.read();
        cout << "Binary: 10101001 -> Decimal: " << result << endl;
        assert(result == 169);

        // Test case 2: Convert binary 1111 to decimal
        binary_sig = 1111;
        wait(1, SC_NS);
        result = decimal_sig.read();
        cout << "Binary: 1111 -> Decimal: " << result << endl;
        assert(result == 15);

        // Test case 3: Convert binary 100000 to decimal
        binary_sig = 100000;
        wait(1, SC_NS);
        result = decimal_sig.read();
        cout << "Binary: 100000 -> Decimal: " << result << endl;
        assert(result == 32);

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
