
#include <systemc.h>

// Module to calculate the order of a number
SC_MODULE(OrderCalculator) {
    sc_in<int> num;  // Input port for the number
    sc_out<int> order; // Output port for the order

    // Constructor
    SC_CTOR(OrderCalculator) {
        // Process to calculate the order
        SC_METHOD(calculate_order);
        sensitive << num;
    }

    // Method to calculate the order of the number
    void calculate_order() {
        int count = 0;
        int temp = num.read();
        while (temp > 0) {
            temp /= 10;
            count++;
        }
        order.write(count);
    }
};

// Module to check if a number is an Armstrong number
SC_MODULE(ArmstrongChecker) {
    sc_in<int> num;  // Input port for the number
    sc_out<bool> is_armstrong; // Output port to indicate if the number is Armstrong

    OrderCalculator order_calculator; // Instance of the OrderCalculator module

    // Constructor
    SC_CTOR(ArmstrongChecker) : order_calculator("order_calc") {
        // Connect signals to ports
        order_calculator.num(num);

        // Process to check if the number is Armstrong
        SC_METHOD(check_armstrong);
        sensitive << num << order_calculator.order;
    }

    // Method to check if the number is Armstrong
    void check_armstrong() {
        int order_n = order_calculator.order.read();
        int num_temp = num.read(), sum = 0;

        int temp = num_temp;
        while (temp > 0) {
            int curr = temp % 10;
            sum += pow(curr, order_n);
            temp /= 10;
        }

        if (sum == num_temp) {
            is_armstrong.write(true);
        } else {
            is_armstrong.write(false);
        }
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> num; // Signal for the number
    sc_signal<bool> is_armstrong; // Signal to indicate if the number is Armstrong

    ArmstrongChecker armstrong_checker; // Instance of the ArmstrongChecker module

    // Constructor
    SC_CTOR(Testbench) : armstrong_checker("armstrong_check") {
        // Connect signals to ports
        armstrong_checker.num(num);
        armstrong_checker.is_armstrong(is_armstrong);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        cout << "Armstrong numbers between 1 to 1000 : ";
        for (int num_val = 1; num_val <= 1000; ++num_val) {
            num = num_val;
            wait(1, SC_NS); // Wait for the checker to process
            if (is_armstrong.read()) {
                cout << num_val << " ";
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
