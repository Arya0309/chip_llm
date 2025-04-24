scss
#include <systemc.h>


SC_MODULE(OctalToDecimalConverter) {

  // Inputs
  sc_in<int> octal_input;

  // Outputs
  sc_out<int> decimal_output;

  // Helper variables
  int num, dec_value, base;

  // Constructor
  SC_CTOR(OctalToDecimalConverter) {
    // Initialize helper variables
    num = 0;
    dec_value = 0;
    base = 1;
  }

  // Function to perform the conversion
  void Convert() {
    // Check if the input is positive
    if (octal_input.read() >= 0) {
      // Set num to the input value
      num = octal_input.read();

      // Iterate through the octal number
      while (num > 0) {
        // Extract the last digit of the input
        int last_digit = num % 10;

        // Multiply the last digit by the appropriate power of 8
        dec_value += last_digit * base;

        // Increment the base variable
        base *= 8;

        // Divide the input by 10
        num /= 10;
      }
    }

    // Set the output to the decimal equivalent of the input
    decimal_output.write(dec_value);
  }
};


SC_MODULE(Testbench) {
    sc_signal<int> octal_input; // Signal for the octal number
    sc_signal<int> decimal_output; // Signal for the decimal number

    OctalToDecimalConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.octal_input(octal_input);
        converter_inst.decimal_output(decimal_output);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Test case 1: Octal 67
        octal_input = 67;
        wait(1, SC_NS); // Wait for the converter to process
        assert(decimal_output.read() == 55);
        cout << "Octal 67 -> Decimal " << decimal_output.read() << endl;

        // Test case 2: Octal 123
        octal_input = 123;
        wait(1, SC_NS);
        assert(decimal_output.read() == 83);
        cout << "Octal 123 -> Decimal " << decimal_output.read() << endl;

        // Test case 3: Octal 0
        octal_input = 0;
        wait(1, SC_NS);
        assert(decimal_output.read() == 0);
        cout << "Octal 0 -> Decimal " << decimal_output.read() << endl;

        // Test case 4: Octal 10
        octal_input = 10;
        wait(1, SC_NS);
        assert(decimal_output.read() == 8);
        cout << "Octal 10 -> Decimal " << decimal_output.read() << endl;

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
