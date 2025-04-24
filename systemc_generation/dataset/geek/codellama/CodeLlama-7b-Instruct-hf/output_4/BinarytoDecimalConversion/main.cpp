scss
#include <systemc.h>

// Module that converts binary to decimal

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary_in;     // Input port for the binary number
    sc_out<int> decimal_out;  // Output port for the decimal number

    // Function to convert binary to decimal
    void binaryToDecimal(int n) {
        int num = n;
        int dec_value = 0;
        int base = 1;

        while (num) {
            int last_digit = num % 10;
            num = num / 10;
            dec_value += last_digit * base;
            base = base * 2;
        }

        // Output the decimal equivalent of the binary number
        decimal_out.write(dec_value);
    }

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        // Declare input and output ports
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
