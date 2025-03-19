
#include <systemc.h>

// Module to calculate the average of an array
SC_MODULE(AverageCalculator) {
    // Input port for the array size
    sc_in<int> n_in;
    
    // Output port for the calculated average
    sc_out<double> avg_out;

    // Internal signal to hold the sum of array elements
    sc_signal<int> sum;

    // Array to store input values
    int *arr;

    // Constructor to initialize the module
    SC_CTOR(AverageCalculator) {
        // Process to calculate the sum of array elements
        SC_METHOD(calculate_sum);
        sensitive << n_in;

        // Process to calculate the average
        SC_METHOD(calculate_average);
        sensitive << sum;
    }

    // Method to calculate the sum of array elements
    void calculate_sum() {
        int n = n_in.read();
        sum = 0;
        for (int i = 0; i < n; i++) {
            sum = sum.read() + arr[i];
        }
    }

    // Method to calculate the average
    void calculate_average() {
        int n = n_in.read();
        double avg = static_cast<double>(sum.read()) / n;
        avg_out.write(avg);
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with AverageCalculator
    sc_signal<int> n_in;
    sc_signal<double> avg_out;

    // Instance of AverageCalculator
    AverageCalculator calc;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the array
        int arr[] = { 10, 2, 3, 4, 5, 6, 7, 8, 9 };
        int n = sizeof(arr) / sizeof(arr[0]);
        calc.arr = arr;

        // Drive the input size
        n_in = n;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Average: " << avg_out.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calc("calc") {
        // Connecting signals to the calculator ports
        calc.n_in(n_in);
        calc.avg_out(avg_out);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
