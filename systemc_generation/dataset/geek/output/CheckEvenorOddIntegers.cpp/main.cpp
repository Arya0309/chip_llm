
#include <systemc.h>

// User-defined module to check if a number is even or odd
SC_MODULE(EvenOddChecker) {
    // Input port for the number to check
    sc_in<int> number;

    // Output port for the result
    sc_out<bool> isEven; // True if even, False if odd

    // Process to determine if the number is even or odd
    void checkEvenOdd() {
        // Read the input number
        int n = number.read();

        // Determine if the number is even
        bool even = (n % 2 == 0);

        // Write the result to the output port
        isEven.write(even);
    }

    // Constructor to register the process
    SC_CTOR(EvenOddChecker) {
        // Sensitivity list: react to changes on the 'number' input
        SC_METHOD(checkEvenOdd);
        sensitive << number;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with EvenOddChecker
    sc_signal<int> numSignal;
    sc_signal<bool> resultSignal;

    // Instance of EvenOddChecker
    EvenOddChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the number to check
        numSignal = 11;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the results
        std::cout << "Number: " << numSignal.read() << " is ";
        if (resultSignal.read()) {
            std::cout << "Even";
        } else {
            std::cout << "Odd";
        }
        std::cout << std::endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.number(numSignal);
        checker.isEven(resultSignal);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
