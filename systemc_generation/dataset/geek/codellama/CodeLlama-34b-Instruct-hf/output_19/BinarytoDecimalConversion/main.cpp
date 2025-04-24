
#include <systemc.h>


SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary_in;  // Input port for the binary number
    sc_out<int> decimal_out;  // Output port for the decimal value

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        // Process to perform binary-to-decimal conversion
        SC_METHOD(binaryToDecimal);
        sensitive << binary_in;
    }

    // Method to perform binary-to-decimal conversion
    void binaryToDecimal() {
        int num = binary_in.read();  // Read the input binary number
        int dec_value = 0;  // Initialize the output decimal value
        int base = 1;  // Initialize the base value
        int temp = num;  // Initialize a temporary variable

        // While loop to perform binary-to-decimal conversion
        while (temp != 0) {
            int last_digit = temp % 10;  // Get the last digit of the binary number
            temp = temp / 10;  // Divide the binary number by 10
            dec_value += last_digit * base;  // Add the product of the last digit and the base to the output decimal value
            base = base * 2;  // Multiply the base by 2
        }

        decimal_out.write(dec_value);  // Write the output decimal value
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
