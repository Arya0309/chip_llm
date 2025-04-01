
#include <systemc.h>

// Module to convert binary to decimal
SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> binary_in;  // Input port for the binary number
    sc_out<int> decimal_out; // Output port for the decimal equivalent

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << binary_in;
    }

    // Method to convert binary to decimal
    void convert() {
        int num = binary_in.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 2;
        }

        decimal_out.write(dec_value);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> binary_in; // Signal for the binary number
    sc_signal<int> decimal_out; // Signal for the decimal equivalent

    BinaryToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.binary_in(binary_in);
        converter_inst.decimal_out(decimal_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert binary 10101001
        binary_in = 10101001;
        wait(1, SC_NS); // Wait for the converter to process
        assert(decimal_out.read() == 177);
        cout << "Binary: " << binary_in.read() << " -> Decimal: " << decimal_out.read() << endl;

        // Test case 2: Convert binary 1111
        binary_in = 1111;
        wait(1, SC_NS);
        assert(decimal_out.read() == 15);
        cout << "Binary: " << binary_in.read() << " -> Decimal: " << decimal_out.read() << endl;

        // Test case 3: Convert binary 10000
        binary_in = 10000;
        wait(1, SC_NS);
        assert(decimal_out.read() == 16);
        cout << "Binary: " << binary_in.read() << " -> Decimal: " << decimal_out.read() << endl;

        // Test case 4: Convert binary 0
        binary_in = 0;
        wait(1, SC_NS);
        assert(decimal_out.read() == 0);
        cout << "Binary: " << binary_in.read() << " -> Decimal: " << decimal_out.read() << endl;

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
