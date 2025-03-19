
#include <systemc.h>

SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_input;  // Input port for octal number
    sc_out<int> decimal_output;  // Output port for decimal number

    void convert() {
        int num = octal_input.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        decimal_output.write(dec_value);
    }

    SC_CTOR(OctalToDecimalConverter) {
        SC_METHOD(convert);
        sensitive << octal_input;
    }
};

int sc_main(int argc, char* argv[]) {
    OctalToDecimalConverter converter("converter");
    sc_signal<int> octal_input_signal;
    sc_signal<int> decimal_output_signal;

    converter.octal_input(octal_input_signal);
    converter.decimal_output(decimal_output_signal);

    octal_input_signal.write(67);  // Set the octal input
    sc_start();  // Start the simulation

    std::cout << "Decimal value: " << decimal_output_signal.read() << std::endl;

    return 0;
}
