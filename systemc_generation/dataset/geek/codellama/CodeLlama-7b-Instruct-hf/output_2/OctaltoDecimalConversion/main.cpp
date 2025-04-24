 SystemC
#include <systemc.h>


SC_MODULE(OctalToDecimalConverter) {
  sc_in<int> octalInput;
  sc_out<int> decimalOutput;

  SC_CTOR(OctalToDecimalConverter) {}

  void concurrentDecoder() {
    sc_uint<32> num, base;
    num = octalInput;
    base = 1;

    // Initializing base value to 1, 
    // i.e 8^0
    while (temp >= 1) {
      // Extracting last digit of input
      sc_uint<32> last_digit = (num % 10);
      temp = (num / 10);

      // Multiplying last digit with 
      // appropriate base value and adding 
      // it to dec_value
      dec_value += last_digit * base;

      base = base * 8;
    }

    // Process output
    decimalOutput = dec_value;
  }
};


SC_MODULE(Testbench) {
    sc_signal<int> octal_input_sig; // Signal for the octal input
    sc_signal<int> decimal_output_sig; // Signal for the decimal output

    OctalToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.octal_input(octal_input_sig);
        converter_inst.decimal_output(decimal_output_sig);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Convert 67 (octal) to decimal
        octal_input_sig = 67;
        wait(1, SC_NS); // Wait for the converter to process
        int result = decimal_output_sig.read();
        cout << "Octal: 67 -> Decimal: " << result << endl;
        assert(result == 55); // 67 in octal is 55 in decimal

        // Additional test case: Convert 123 (octal) to decimal
        octal_input_sig = 123;
        wait(1, SC_NS);
        result = decimal_output_sig.read();
        cout << "Octal: 123 -> Decimal: " << result << endl;
        assert(result == 83); // 123 in octal is 83 in decimal

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
