
#include <systemc.h>

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<sc_uint<8>> binary_in;  // Input: Binary number (up to 8 bits)
    sc_out<int> decimal_out;      // Output: Decimal number

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

    SC_CTOR(BinaryToDecimalConverter) {
        SC_METHOD(convert);
        sensitive << binary_in;
    }
};

int sc_main(int argc, char* argv[]) {
    BinaryToDecimalConverter converter("converter");
    sc_signal<sc_uint<8>> binary_input;
    sc_signal<int> decimal_output;

    converter.binary_in(binary_input);
    converter.decimal_out(decimal_output);

    binary_input.write(10101001);  // Example input

    sc_start();  // Start the simulation

    cout << "Binary: 10101001 -> Decimal: " << decimal_output.read() << endl;

    return 0;
}
