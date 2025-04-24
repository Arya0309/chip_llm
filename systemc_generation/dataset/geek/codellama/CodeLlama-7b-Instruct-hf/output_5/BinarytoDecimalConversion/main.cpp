
#include <systemc.h>


SC_MODULE(BinaryToDecimalConverter) {
  // Declare the input and output ports.
  sc_in<int> binary_in; 
  sc_out<int> decimal_out;

  // Declare the base value and decimal value variables.
  int base = 1; 
  int dec_value = 0;

  // Declare the temporary variable for the binary number.
  int temp;

  // Create a method to perform the binary to decimal conversion.
  void binary_to_decimal();

  // Constructor
  SC_CTOR(BinaryToDecimalConverter) {
    // Bind the method to the binary_to_decimal_out port.
    SC_METHOD(binary_to_decimal)
      sensitive << binary_in;
    }

  void binary_to_decimal() {
    // If the binary input is 0, return 0.
    if (binary_in == 0) {
      decimal_out.write(0);
      return;
    }

    // Initialize the temp variable to the binary input.
    temp = binary_in;

    // Create a while loop to iterate over the binary input.
    while (temp != 0) {
      // Get the last digit of the binary input.
      int last_digit = temp % 10;
      // Remove the last digit from the binary input.
      temp = temp / 10;
      // Update the decimal value with the last digit multiplied by the base value.
      dec_value = dec_value + last_digit * base;
      // Update the base value with the product of the base value multiplied by 2.
      base = base * 2;
    }

    // Return the decimal value.
    decimal_out.write(dec_value);
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
