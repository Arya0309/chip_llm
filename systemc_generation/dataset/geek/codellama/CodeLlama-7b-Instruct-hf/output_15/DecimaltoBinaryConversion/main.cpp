
#include <systemc.h>

// Declare the input and output channels
sc_in<int> dec_in;
sc_out<int> binary_out;

// Define the binary to decimal function ()
void DecToBinary(int n) {
	// Define an array to store the binary number
	int binaryNum[32];

	// Define a counter for the binary array
	int i = 0;

	// While the user's input is greater than 0
	while (n > 0) {
		// Store the remainder in the binary array
		binaryNum[i] = n % 2;
		n = n / 2;
		i++;
	}

	// Print the binary array in reverse order
	for (int j = i - 1; j >= 0; j--)
		cout << binaryNum[j];
}

// Define the main module

SC_MODULE(DecToBinary) {
    // Define ports for the input channel
    sc_in<int> dec_in;

    // Define ports for the output channel
    sc_out<int> binary_out;

    // Define a counter variable that keeps track of the binary array
    int i = 0;

    // Define a method that calls the DecToBinary function ()
    void DecToBinary();

    // Define a method that is called at the end of the conversion process to display the binary result
    void displayBinary();

    // Define a constructor that creates an object and assigns the input and output channels to it and calls the DecToBinary and displayBinary functions ()
    SC_CTOR(DecToBinary) {
        // Assign the binary_out port of the DecToBinary function () to the output channel "binary_out"
        sc_out<int> binary_out("binary_out");

        // Assign the dec_in port of the DecToBinary function () to the input channel "dec_in"
        sc_in<int> dec_in("dec_in");

        // Invoke the DecToBinary function () with the frontend and backend channels
        DecToBinary();

        // Invoke the displayBinary function () with the frontend and backend channels
        displayBinary();
    }
}

// Define a method that will convert the decimal to binary
void DecToBinary() {
	// Define a variable to store the converted binary number
	int binaryNum;

	// Perform the binary conversion of the input integer
	binaryNum = DecToBinary(dec_in.read());

	// Print the binary number
	cout << binaryNum;
}

// Define a method that will print the binary number
void displayBinary() {
	// Define a variable to store the converted binary number
	int binaryNum;

	// Define a counter variable that keeps track of the binary array
	int j = 0;

	// Print the binary number in reverse order
	for (j = i - 1; j >= 0; j--)
		cout << binaryNum[j];
}


SC_MODULE(Testbench) {
    sc_signal<int> dec_sig;    // Signal for the decimal input
    sc_signal<int> bin_sig;    // Signal for the binary output

    DecToBinary* dec_to_binary_inst;  // Instance of the conversion module

    SC_CTOR(Testbench) {
        // Instantiate the DecToBinary module and connect signals
        dec_to_binary_inst = new DecToBinary("DecToBinary");
        dec_to_binary_inst->dec_in(dec_sig);
        dec_to_binary_inst->binary_out(bin_sig);

        SC_THREAD(run_tests);   // Launch test cases in a thread
    }

    void run_tests() {
        // Test Case: Input n = 10 should result in binary 1010
        dec_sig.write(10);
        wait(1, SC_NS);  // Wait for the process to trigger

        int result = bin_sig.read();
        std::cout << "Input: 10, Binary Output: " << result << std::endl;
        assert(result == 1010);

        // Test Case: Input n = 0 should result in binary 0
        dec_sig.write(0);
        wait(1, SC_NS);  // Wait for the process to trigger

        int result2 = bin_sig.read();
        std::cout << "Input: 0, Binary Output: " << result2 << std::endl;
        assert(result2 == 0);

        // Test Case: Input n = 1000 should result in binary 1111101000
        dec_sig.write(1000);
        wait(1, SC_NS);  // Wait for the process to trigger
        int result3 = bin_sig.read();
        std::cout << "Input: 1000, Binary Output: " << result3 << std::endl;
        assert(result3 == 1111101000);

        // Print success message and stop simulation
        std::cout << "Test passed successfully." << std::endl;
        sc_stop();
    }

    // Destructor to free allocated memory
    ~Testbench() {
        delete dec_to_binary_inst;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");  // Create an instance of the Testbench module
    sc_start();          // Start the simulation
    return 0;
}
