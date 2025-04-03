
#include <systemc.h>

// Module to check if a number is an Armstrong number
SC_MODULE(ArmstrongChecker) {
    sc_in<int> number_in;       // Input port for the number to check
    sc_out<bool> is_armstrong; // Output port indicating if the number is an Armstrong number

    // Constructor
    SC_CTOR(ArmstrongChecker) {
        // Register the method to check Armstrong number
        SC_METHOD(check_armstrong);
        sensitive << number_in;
    }

    // Method to check if the number is an Armstrong number
    void check_armstrong() {
        int num = number_in.read();
        int order_n = order(num);
        int num_temp = num, sum = 0;

        while (num_temp > 0) {
            int curr = num_temp % 10;
            sum += pow(curr, order_n);
            num_temp /= 10;
        }

        is_armstrong.write(sum == num);
    }

    // Function to return the order of a number
    int order(int num) {
        int count = 0;
        while (num > 0) {
            num /= 10;
            count++;
        }
        return count;
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> number_sig;       // Signal for the number to check
    sc_signal<bool> armstrong_sig;   // Signal indicating if the number is an Armstrong number

    // Instance of the ArmstrongChecker module
    ArmstrongChecker checker_inst;

    // Constructor
    SC_CTOR(Testbench) : checker_inst("checker") {
        // Connect signals to module ports
        checker_inst.number_in(number_sig);
        checker_inst.is_armstrong(armstrong_sig);

        // Create a process to iterate through numbers and check Armstrong condition
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        cout << "Armstrong numbers between 1 to 1000 : ";
        for (int num = 1; num <= 1000; ++num) {
            number_sig.write(num);
            wait(1, SC_NS); // Wait for the checker to process
            if (armstrong_sig.read()) {
                cout << num << " ";
            }
        }
        cout << endl;
        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
