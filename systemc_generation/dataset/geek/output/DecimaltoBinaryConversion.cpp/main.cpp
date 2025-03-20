
#include <systemc.h>

// Module to convert decimal to binary
SC_MODULE(DecToBinConverter) {
    // Input port for decimal number
    sc_in<int> decimal_in;
    
    // Output port for binary string
    sc_out<sc_bv<32>> binary_out;

    // Process to convert decimal to binary
    void convertDecimalToBinary() {
        int n = decimal_in.read();
        sc_bv<32> binaryNum = 0; // Initialize binary number as 0
        
        // Counter for binary array
        int i = 0;
        while (n > 0) {
            // Storing remainder in binary array
            binaryNum[i] = n % 2;
            n = n / 2;
            i++;
        }
        
        // Write the binary number to output port
        binary_out.write(binaryNum);
    }

    // Constructor to register the process
    SC_CTOR(DecToBinConverter) {
        SC_METHOD(convertDecimalToBinary);
        sensitive << decimal_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with DecToBinConverter
    sc_signal<int> decimal_in;
    sc_signal<sc_bv<32>> binary_out;

    // Instance of DecToBinConverter
    DecToBinConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing decimal number
        decimal_in = 10;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the binary result
        sc_bv<32> binary_result = binary_out.read();
        cout << "Decimal: 10 -> Binary: ";
        for (int j = 31; j >= 0; j--) {
            if (binary_result[j] != '0') { // Only print non-zero bits
                cout << binary_result[j];
            }
        }
        cout << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.decimal_in(decimal_in);
        converter.binary_out(binary_out);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
