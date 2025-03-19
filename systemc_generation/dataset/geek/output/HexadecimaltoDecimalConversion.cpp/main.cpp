
#include <systemc.h>

SC_MODULE(HexToDecConverter) {
    sc_in<sc_bv<8>> hex_in;  // Input port for hexadecimal value (up to 2 characters)
    sc_out<int> dec_out;     // Output port for decimal value

    void convert() {
        std::string hexStr = hex_in.read().to_string();
        int len = hexStr.size();
        int base = 1;
        int dec_val = 0;

        // Process each character from the end to the start
        for (int i = len - 1; i >= 0; i--) {
            if (hexStr[i] >= '0' && hexStr[i] <= '9') {
                dec_val += (int(hexStr[i]) - 48) * base;
                base *= 16;
            } else if (hexStr[i] >= 'A' && hexStr[i] <= 'F') {
                dec_val += (int(hexStr[i]) - 55) * base;
                base *= 16;
            }
        }

        dec_out.write(dec_val);
    }

    SC_CTOR(HexToDecConverter) {
        SC_METHOD(convert);
        sensitive << hex_in;
    }
};

int sc_main(int argc, char* argv[]) {
    HexToDecConverter converter("converter");

    sc_signal<sc_bv<8>> hex_input;
    sc_signal<int> dec_output;

    converter.hex_in(hex_input);
    converter.dec_out(dec_output);

    // Example input
    hex_input.write(sc_bv<8>("001A"));  // Binary representation of "1A"

    sc_start();

    std::cout << "Hexadecimal 1A is decimal " << dec_output.read() << std::endl;

    return 0;
}
