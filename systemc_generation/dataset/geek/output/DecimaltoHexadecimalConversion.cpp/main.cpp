
#include <systemc.h>

SC_MODULE(DecimalToHex) {
    sc_in<int> decimal_in;  // Input port for decimal number
    sc_out<sc_bv<32>> hex_out;  // Output port for hexadecimal string (as binary vector)

    SC_CTOR(DecimalToHex) {
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    void convert() {
        int n = decimal_in.read();
        char hexaDeciNum[100];
        int i = 0;

        while (n != 0) {
            int temp = n % 16;
            if (temp < 10) {
                hexaDeciNum[i] = temp + 48;
            } else {
                hexaDeciNum[i] = temp + 55;
            }
            i++;
            n = n / 16;
        }

        // Constructing the hexadecimal string
        std::string hexString = "";
        for (int j = i - 1; j >= 0; j--) {
            hexString += hexaDeciNum[j];
        }

        // Converting the hexadecimal string to a binary vector for SystemC output
        sc_bv<32> hexOutBv(hexString.c_str());
        hex_out.write(hexOutBv);
    }
};

int sc_main(int argc, char* argv[]) {
    DecimalToHex dec_to_hex("DecimalToHex");
    sc_signal<int> decimal_in;
    sc_signal<sc_bv<32>> hex_out;

    dec_to_hex.decimal_in(decimal_in);
    dec_to_hex.hex_out(hex_out);

    // Test the module with a sample input
    decimal_in.write(2545);
    sc_start();

    // Print the result
    std::cout << "Decimal: 2545 -> Hexadecimal: " << hex_out.read().to_string() << std::endl;

    return 0;
}
