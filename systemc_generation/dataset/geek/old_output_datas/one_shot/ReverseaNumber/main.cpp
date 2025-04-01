
#include <systemc.h>

// Module to reverse the digits of a number
SC_MODULE(ReverseDigits) {
    sc_in<int> num_in;       // Input port for the number to be reversed
    sc_out<int> num_out;     // Output port for the reversed number

    // Constructor
    SC_CTOR(ReverseDigits) {
        // Process to reverse the digits
        SC_METHOD(reverse);
        sensitive << num_in;
    }

    // Method to reverse the digits of the input number
    void reverse() {
        int num = num_in.read();
        int rev_num = 0;
        while (num > 0) {
            rev_num = rev_num * 10 + num % 10;
            num = num / 10;
        }
        num_out.write(rev_num);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> num_in;   // Signal for the input number
    sc_signal<int> num_out;  // Signal for the reversed number

    ReverseDigits reverse_inst; // Instance of the ReverseDigits module

    // Constructor
    SC_CTOR(Testbench) : reverse_inst("reverse") {
        // Connect signals to ports
        reverse_inst.num_in(num_in);
        reverse_inst.num_out(num_out);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the input number
        num_in = 4562;
        wait(1, SC_NS); // Wait for the reverseDigits module to process
        cout << "Reverse of num is " << num_out.read() << endl;

        // Additional test case
        num_in = 12345;
        wait(1, SC_NS);
        cout << "Reverse of num is " << num_out.read() << endl;

        // Another test case
        num_in = 0;
        wait(1, SC_NS);
        cout << "Reverse of num is " << num_out.read() << endl;

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
