scss
#include <systemc.h>


SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary;  // Input port for the binary number
    sc_out<int> decimal; // Output port for the decimal equivalent

    // Function to convert binary to decimal
    void binaryToDecimal() {
        // Initialize the decimal value to 0
        int decimal_value = 0;

        // Initialize the base to 1
        int base = 1;

        // Copy the binary number to a temporary variable
        int temp = binary_in;

        // Repeatedly extract the last digit of temp and multiply it by base
        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            decimal_value += last_digit * base;
            base = base * 2;
        }

        // Return the final decimal value
        decimal_out.write(decimal_value);
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
