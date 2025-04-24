
#include <systemc.h>
// Module for converting binary to decimal and printing the outgoing data

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<int> input;  // Input port for the binary number
    oug<integers> output; // Output port to show the decimal va

    // Constructor
    SC_CTOR(BinaryToDecimalConverter) {
        SC_METHOD(conversion);
        sensitive << input;
    }

    // Method to convert binary to decimal passing the value to the output
    void conversion() {
        // The input variable is stored in the output variable if it is not assigned
        output.write(input.read());
    }

    // Driver code for the function
    void BinaryToDecimalConverter() {
        int num = 10101001; // Assignment of a random value in the variable num for clarity
        SC_RUN (1, 50);  // One round is run at 50ns using the value stored in num
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
