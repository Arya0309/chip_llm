
#include <systemc.h>

// Module to convert decimal to binary
SC_MODULE(DecToBinConverter) {
    sc_in<int> decimal_in;  // Input port for the decimal number
    sc_out<sc_bv<32>> binary_out; // Output port for the binary number

    // Constructor
    SC_CTOR(DecToBinConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    // Method to convert decimal to binary
    void convert() {
        int n = decimal_in.read();
        sc_bv<32> binaryNum(0); // Initialize binary number to 0

        // Counter for binary array
        int i = 0;
        while (n > 0) {
            // Storing remainder in binary
            // array
            binaryNum[i] = n % 2;
            n = n / 2;
            i++;
        }

        // Set the remaining bits to 0
        for (int j = i; j < 32; j++) {
            binaryNum[j] = 0;
        }

        // Write the binary number to the output port
        binary_out.write(binaryNum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> decimal_in; // Signal for the decimal number
    sc_signal<sc_bv<32>> binary_out; // Signal for the binary number

    DecToBinConverter converter_inst; // Instance of the DecToBinConverter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.decimal_in(decimal_in);
        converter_inst.binary_out(binary_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 10 to binary
        decimal_in = 10;
        wait(1, SC_NS); // Wait for the converter to process
        cout << "Decimal: 10 -> Binary: " << binary_out.read() << endl;

        // Test case 2: Convert 0 to binary
        decimal_in = 0;
        wait(1, SC_NS);
        cout << "Decimal: 0 -> Binary: " << binary_out.read() << endl;

        // Test case 3: Convert 255 to binary
        decimal_in = 255;
        wait(1, SC_NS);
        cout << "Decimal: 255 -> Binary: " << binary_out.read() << endl;

        // Test case 4: Convert 1023 to binary
        decimal_in = 1023;
        wait(1, SC_NS);
        cout << "Decimal: 1023 -> Binary: " << binary_out.read() << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
