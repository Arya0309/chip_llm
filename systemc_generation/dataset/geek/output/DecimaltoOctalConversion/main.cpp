
#include <systemc.h>

// Module to convert a decimal number to an octal number
SC_MODULE(DecimalToOctalConverter) {
    sc_in<int> decimal_in;  // Input port for the decimal number
    sc_out<sc_bv<32>> octal_out; // Output port for the octal number

    // Constructor
    SC_CTOR(DecimalToOctalConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    // Method to convert decimal to octal
    void convert() {
        int n = decimal_in.read();
        sc_bv<32> octalNum; // Use a bit vector to store octal digits
        int i = 0;

        // Array to store octal number
        int octalArray[100];

        // Convert decimal to octal
        while (n != 0) {
            octalArray[i] = n % 8;
            n = n / 8;
            i++;
        }

        // Store octal digits in a bit vector
        octalNum = 0; // Clear the bit vector
        for (int j = i - 1; j >= 0; j--) {
            octalNum.range((j + 1) * 3 - 1, j * 3) = octalArray[j];
        }

        // Output the octal number
        octal_out.write(octalNum);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> decimal_in; // Signal for the decimal number
    sc_signal<sc_bv<32>> octal_out; // Signal for the octal number

    DecimalToOctalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.decimal_in(decimal_in);
        converter_inst.octal_out(octal_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 33 to octal
        decimal_in = 33;
        wait(1, SC_NS); // Wait for the conversion to process
        std::cout << "Decimal: " << decimal_in.read() << " -> Octal: " << octal_out.read().to_string(SC_BIN).c_str() << std::endl;

        // Additional test case: Convert 0 to octal
        decimal_in = 0;
        wait(1, SC_NS);
        std::cout << "Decimal: " << decimal_in.read() << " -> Octal: " << octal_out.read().to_string(SC_BIN).c_str() << std::endl;

        // Additional test case: Convert 255 to octal
        decimal_in = 255;
        wait(1, SC_NS);
        std::cout << "Decimal: " << decimal_in.read() << " -> Octal: " << octal_out.read().to_string(SC_BIN).c_str() << std::endl;

        // End simulation after running tests
        std::cout << "Tests completed." << std::endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
