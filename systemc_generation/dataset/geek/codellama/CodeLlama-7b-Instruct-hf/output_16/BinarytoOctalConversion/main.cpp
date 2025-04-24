
#include <systemc.h>

// The octal to decimal conversion function
int octalToDecimal(int n)
{
	int num = n;
	int dec_value = 0;

	// Initializing base value to 1, 
	// i.e 8^0
	int base = 1;

	int temp = num;
	while (temp) 
	{
		// Extracting last digit
		int last_digit = temp % 10;
		temp = temp / 10;

		// Multiplying last digit with 
		// appropriate base value and adding 
		// it to dec_value
		dec_value += last_digit * base;

		base = base * 8;
	}

	return dec_value;
}

// The SystemC module

SC_MODULE(OctalToDecimalConverter)
{
  // Input and output ports
  sc_in<int> octal_input;
  sc_out<int> decimal_output;

  // The behavior graph
  sc_solve( 
    // Call the octalToDecimal function with the input
    octalToDecimal(octal_input.read(), decimal_output.write());
  );

  // The conversion function
  void octalToDecimal(int n)
  {
    // Implement the logic of the original function
    int num = n;
    int dec_value = 0;

    // Initializing base value to 1, 
    // i.e 8^0
    int base = 1;

    int temp = num;
    while (temp) 
    {
      // Extracting last digit
      int last_digit = temp % 10;
      temp = temp / 10;

      // Multiplying last digit with 
      // appropriate base value and adding 
      // it to dec_value
      dec_value += last_digit * base;

      base = base * 8;
    }

    return dec_value;
  }
};

// Main program
int main()
{
  // Create the module instance
  OctalToDecimalConverter module("my_octal_to_decimal_module");

  // Declare the input and output signals
  sc_signal<int> octal_input;
  sc_signal<int> decimal_output;

  // Connect the input and output signals to the module ports
  octal_input.bind(module.octal_input);
  decimal_output.bind(module.decimal_output);

  // Run the simulation with the correct values
  octal_input = 67;
  decimal_output = 0;
  module.at_posedge();

  // Print the output
  cout << "The decimal value of 67 in octal is: " << decimal_output.read() << endl;

  return 0;
}


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
