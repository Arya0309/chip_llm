
#include <systemc.h>
#include <cmath>

// Module to check Armstrong numbers
SC_MODULE(ArmstrongChecker) {
    sc_in<int> num_in;          // Input port for the number to check
    sc_out<bool> is_armstrong;  // Output port indicating if the number is an Armstrong number

    // Internal signal for the order of the number
    int order_n;

    // Process to determine the order of the number
    void calculate_order() {
        int num = num_in.read();
        order_n = 0;
        while (num > 0) {
            num /= 10;
            order_n++;
        }
    }

    // Process to check if the number is an Armstrong number
    void check_armstrong() {
        int num = num_in.read();
        int num_temp = num, sum = 0;

        while (num_temp > 0) {
            int curr = num_temp % 10;
            sum += pow(curr, order_n);
            num_temp /= 10;
        }
        if (sum == num) {
            is_armstrong.write(true);
        } else {
            is_armstrong.write(false);
        }
    }

    // Constructor to register the processes
    SC_CTOR(ArmstrongChecker) {
        SC_METHOD(calculate_order);
        sensitive << num_in;

        SC_METHOD(check_armstrong);
        sensitive << num_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with ArmstrongChecker
    sc_signal<int> num;
    sc_signal<bool> armstrong_flag;

    // Instance of ArmstrongChecker
    ArmstrongChecker checker;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        cout << "Armstrong numbers between 1 to 1000 : ";
        for (int i = 1; i <= 1000; ++i) {
            num = i;
            wait(1, SC_NS);  // Wait for the checker to process the input
            if (armstrong_flag.read()) {
                cout << i << " ";
            }
        }
        sc_stop();  // Stop the simulation after checking all numbers
    }

    // Constructor to register the process and connect signals
    SC_CTOR(Testbench) : checker("checker") {
        checker.num_in(num);
        checker.is_armstrong(armstrong_flag);

        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();  // Start simulation
    return 0;
}
