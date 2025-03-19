
#include <systemc.h>

// SystemC module to convert octal to decimal
SC_MODULE(OctalToDecimalConverter) {
    sc_in<int> octal_in;  // Input port for octal number
    sc_out<int> decimal_out;  // Output port for decimal number

    void conversion_process() {
        int num = octal_in.read();
        int dec_value = 0;
        int base = 1;
        int temp = num;

        while (temp) {
            int last_digit = temp % 10;
            temp = temp / 10;
            dec_value += last_digit * base;
            base = base * 8;
        }

        decimal_out.write(dec_value);
    }

    SC_CTOR(OctalToDecimalConverter) {
        SC_METHOD(conversion_process);
        sensitive << octal_in;
    }
};

// Testbench for OctalToDecimalConverter
SC_MODULE(Testbench) {
    sc_signal<int> octal_signal;
    sc_signal<int> decimal_signal;

    OctalToDecimalConverter converter{"converter"};
    
    SC_CTOR(Testbench) {
        converter.octal_in(octal_signal);
        converter.decimal_out(decimal_signal);

        SC_THREAD(test_process);
    }

    void test_process() {
        octal_signal = 67;  // Input octal number
        wait(1, SC_NS);  // Wait for the conversion process to complete
        cout << "Octal: 67 -> Decimal: " << decimal_signal.read() << endl;
        sc_stop();  // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb{"tb"};
    sc_start();  // Start the simulation
    return 0;
}
