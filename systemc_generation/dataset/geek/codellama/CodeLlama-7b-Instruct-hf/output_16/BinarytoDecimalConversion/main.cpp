
#include <systemc.h>

// Module that converts binary to decimal

SC_MODULE(BinaryToDecimalConverter) {
    sc_in<sc_int<32>> binary_in; // input port for binary number
    sc_out<sc_int<32>> decimal_out; // output port for decimal equivalent

    // Function to convert binary to decimal
    void binaryToDecimal(int n) {
        int num = n;
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
};

// Module that serves as a testbench for the BinaryToDecimalConverter module

SC_MODULE(BinaryToDecimalConverterTestbench) {
    SC_CTOR(BinaryToDecimalConverterTestbench) {
        BinaryToDecimalConverter btd;
        sc_in<sc_int<32>> binary_in("binary_in");
        sc_out<sc_int<32>> decimal_out("decimal_out");
        binary_in(btd.binary_in);
        decimal_out(btd.decimal_out);
        SC_THREAD(run_test);
    }

    void run_test() {
        binary_in.write(10101001);
        sc_start(10, SC_NS);
        sc_assert(decimal_out.read() == 43);
        sc_stop();
    }
};


SC_MODULE(Testbench) {
    sc_signal<int> binary_sig; // Signal for the binary number
    sc_signal<int> decimal_sig; // Signal for the decimal number

    BinaryToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.binary_in(binary_sig);
        converter_inst.decimal_out(decimal_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert binary 10101001 to decimal
        binary_sig = 10101001;
        wait(1, SC_NS); // Wait for the conversion to process
        int result = decimal_sig.read();
        cout << "Binary: 10101001 -> Decimal: " << result << endl;
        assert(result == 169);

        // Test case 2: Convert binary 1111 to decimal
        binary_sig = 1111;
        wait(1, SC_NS);
        result = decimal_sig.read();
        cout << "Binary: 1111 -> Decimal: " << result << endl;
        assert(result == 15);

        // Test case 3: Convert binary 100000 to decimal
        binary_sig = 100000;
        wait(1, SC_NS);
        result = decimal_sig.read();
        cout << "Binary: 100000 -> Decimal: " << result << endl;
        assert(result == 32);

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
