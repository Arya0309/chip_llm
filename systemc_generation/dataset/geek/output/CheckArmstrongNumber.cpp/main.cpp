
#include <systemc.h>

// Module to check if a number is an Armstrong number
SC_MODULE(ArmstrongChecker) {
    // Input port for the number to check
    sc_in<int> number;
    
    // Output port for the result
    sc_out<bool> is_armstrong;

    // Process to check if the number is an Armstrong number
    void checkArmstrong() {
        int n = number.read();
        int temp = n;
        int p = 0;

        // Calculate the sum of cubes of digits
        while (n > 0) {
            int rem = n % 10;
            p += (rem * rem * rem);
            n /= 10;
        }

        // Check if the number is an Armstrong number
        bool result = (temp == p);
        is_armstrong.write(result);
    }

    // Constructor to register the process
    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(checkArmstrong);
        sensitive << number;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with ArmstrongChecker
    sc_signal<int> num;
    sc_signal<bool> result;

    // Instance of ArmstrongChecker
    ArmstrongChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the number to check
        num = 153;

        // Wait for one delta cycle to let the checker process
        wait(1, SC_NS);

        // Print the results
        if (result.read()) {
            cout << "Yes. It is Armstrong No." << endl;
        } else {
            cout << "No. It is not an Armstrong No." << endl;
        }
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : checker("checker") {
        // Connecting signals to the checker ports
        checker.number(num);
        checker.is_armstrong(result);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
