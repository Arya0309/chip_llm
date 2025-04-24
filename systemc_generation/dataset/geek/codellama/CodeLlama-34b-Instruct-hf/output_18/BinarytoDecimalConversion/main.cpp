c++
#include <systemc.h>

// Module that converts binary to decimal

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary_in;  // Input port for the binary number
    sc_out<int> decimal_out;  // Output port for the decimal value

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        // Process to perform the conversion
        SC_METHOD(convert);
        sensitive << binary_in;
    }

    // Function to convert the binary number to decimal
    void convert() {
        int num = binary_in.read();

        // Initialize the decimal value and base
        int dec_value = 0;
        int base = 1;

        // Iterate through the binary representation of the number
        while (num) {
            // Extract the last digit of the binary representation
            int last_digit = num % 10;

            // Add the last digit to the decimal value
            dec_value += last_digit * base;

            // Increment the base
            base *= 2;

            // Shift the binary representation to the right
            num /= 10;
        }

        // Output the decimal value
        decimal_out.write(dec_value);
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
