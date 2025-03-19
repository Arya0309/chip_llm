
#include <systemc.h>

// Module to convert decimal to octal
SC_MODULE(DecToOctalConverter) {
    // Input port for decimal number
    sc_in<int> decimalIn;

    // Output port for octal number (as a string for simplicity)
    sc_out<sc_bv<32>> octalOut;

    // Process to perform the conversion
    void convert() {
        int n = decimalIn.read();
        sc_bv<32> octalNum; // Assuming a maximum of 32 bits for the octal representation
        int i = 0;

        // Conversion logic
        while (n != 0) {
            octalNum.range((i+1)*3-1, i*3) = n % 8; // Store each octal digit in 3-bit chunks
            n = n / 8;
            i++;
        }

        // Shift the octal digits to the left to align them properly
        octalNum <<= (32 - i*3);

        // Write the result to the output port
        octalOut.write(octalNum);
    }

    // Constructor to register the process
    SC_CTOR(DecToOctalConverter) {
        SC_METHOD(convert);
        sensitive << decimalIn;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with DecToOctalConverter
    sc_signal<int> decimalSignal;
    sc_signal<sc_bv<32>> octalSignal;

    // Instance of DecToOctalConverter
    DecToOctalConverter converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the decimal number
        decimalSignal = 33;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        cout << "Decimal number: " << decimalSignal.read() << endl;
        cout << "Octal number: " << octalSignal.read().to_uint() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.decimalIn(decimalSignal);
        converter.octalOut(octalSignal);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
