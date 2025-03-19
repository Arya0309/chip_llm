
#include <systemc.h>

SC_MODULE(DecimalToBinaryConverter) {
    sc_in<int> decimal_in;  // Input port for decimal number
    sc_out<sc_bv<32>> binary_out;  // Output port for binary number

    SC_CTOR(DecimalToBinaryConverter) {
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    void convert() {
        int n = decimal_in.read();
        sc_bv<32> binaryNum;  // Bit vector to store binary number
        int i = 0;

        while (n > 0) {
            binaryNum[i] = n % 2;  // Store the least significant bit
            n = n / 2;  // Shift right
            i++;
        }

        // Fill remaining bits with 0s
        for (int j = i; j < 32; j++) {
            binaryNum[j] = 0;
        }

        binary_out.write(binaryNum);  // Write the binary number to the output port
    }
};

int sc_main(int argc, char* argv[]) {
    DecimalToBinaryConverter converter("converter");
    sc_signal<int> decimal_in;
    sc_signal<sc_bv<32>> binary_out;

    converter.decimal_in(decimal_in);
    converter.binary_out(binary_out);

    decimal_in.write(10);  // Set the input decimal number

    sc_start();  // Start the simulation

    std::cout << "Binary representation: " << binary_out.read() << std::endl;

    return 0;
}
