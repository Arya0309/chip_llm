
#include <systemc.h>

// Module to reverse digits of an integer
SC_MODULE(DigitReverser) {
    // Input port for the number to be reversed
    sc_in<int> num_in;
    
    // Output port for the reversed number
    sc_out<int> num_out;

    // Process to reverse the digits of the input number
    void reverseDigits() {
        int num = num_in.read();
        int rev_num = 0;
        
        // Reverse the digits of num
        while (num > 0) {
            rev_num = rev_num * 10 + num % 10;
            num = num / 10;
        }
        
        // Write the reversed number to the output port
        num_out.write(rev_num);
    }

    // Constructor to register the process
    SC_CTOR(DigitReverser) {
        SC_METHOD(reverseDigits);
        sensitive << num_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with DigitReverser
    sc_signal<int> num;
    sc_signal<int> reversed_num;

    // Instance of DigitReverser
    DigitReverser reverser;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the number
        num = 4562;

        // Wait for one delta cycle to let the reverser process
        wait(1, SC_NS);

        // Print the results
        cout << "Original number: " << num.read() << endl;
        cout << "Reversed number: " << reversed_num.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : reverser("reverser") {
        // Connecting signals to the reverser ports
        reverser.num_in(num);
        reverser.num_out(reversed_num);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
