
#include <systemc.h>

// SystemC module to calculate the sum of first n natural numbers
SC_MODULE(SumCalculator) {
    // Input port for the number n
    sc_in<int> n;
    
    // Output port for the sum
    sc_out<int> sum;

    // Internal signal to store the sum during computation
    sc_signal<int> internal_sum;

    // Process to compute the sum of first n natural numbers
    void computeSum() {
        // Initialize sum to 0
        internal_sum.write(0);
        
        // Wait for one delta cycle to ensure n is available
        wait(1, SC_NS);
        
        // Read the value of n
        int num = n.read();
        
        // Calculate the sum of first n natural numbers
        int total_sum = 0;
        for (int i = 1; i <= num; ++i) {
            total_sum += i;
        }
        
        // Write the result to the output port
        sum.write(total_sum);
    }

    // Constructor to register the process
    SC_CTOR(SumCalculator) {
        SC_THREAD(computeSum);
        sensitive << n;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with SumCalculator
    sc_signal<int> n;
    sc_signal<int> sum;

    // Instance of SumCalculator
    SumCalculator calculator;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Set the value of n
        n.write(5);
        
        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);
        
        // Print the result
        cout << "Sum of first " << n.read() << " natural numbers: " << sum.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : calculator("calculator") {
        // Connecting signals to the calculator ports
        calculator.n(n);
        calculator.sum(sum);
        
        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
