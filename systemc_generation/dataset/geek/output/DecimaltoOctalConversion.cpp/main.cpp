
#include <systemc.h>

SC_MODULE(DecimalToOctalConverter) {
    sc_in<int> decimal_in;
    sc_out<sc_bv<32>> octal_out;

    SC_CTOR(DecimalToOctalConverter) {
        SC_METHOD(convert);
        sensitive << decimal_in;
    }

    void convert() {
        int n = decimal_in.read();
        int octalNum[100];
        int i = 0;

        // Conversion process
        while (n != 0) {
            octalNum[i] = n % 8;
            n = n / 8;
            i++;
        }

        // Constructing the octal output bit vector
        sc_bv<32> octal_output = 0;
        int shift = 0;
        for (int j = i - 1; j >= 0; j--) {
            octal_output.range(shift + 3, shift) = octalNum[j];
            shift += 4; // Assuming each octal digit is represented by 4 bits
        }

        octal_out.write(octal_output);
    }
};

SC_MODULE(TopModule) {
    sc_signal<int> decimal_signal;
    sc_signal<sc_bv<32>> octal_signal;

    DecimalToOctalConverter converter;

    SC_CTOR(TopModule) : converter("converter") {
        converter.decimal_in(decimal_signal);
        converter.octal_out(octal_signal);

        SC_THREAD(drive_input);
        sensitive << sc_start_of_simulation_event();

        SC_METHOD(print_output);
        sensitive << octal_signal;
    }

    void drive_input() {
        decimal_signal = 33;
        wait(1, SC_NS);
        sc_stop();
    }

    void print_output() {
        sc_bv<32> octal_value = octal_signal.read();
        std::cout << "Octal representation: " << octal_value.to_string(SC_BIN).c_str() << std::endl;
    }
};

int sc_main(int argc, char* argv[]) {
    TopModule top("top");
    sc_start();
    return 0;
}
