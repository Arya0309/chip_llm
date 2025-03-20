
#include <systemc.h>

// Module to add two numbers
SC_MODULE(SimpleAdder) {
    // Input ports
    sc_in<int> a_in;
    sc_in<int> b_in;
    
    // Output port
    sc_out<int> sum_out;

    // Process to add two numbers
    void addNumbers() {
        // Reading inputs
        int a = a_in.read();
        int b = b_in.read();

        // Calculating the sum
        int sum = a + b;

        // Writing the output
        sum_out.write(sum);
    }

    // Constructor to register the process
    SC_CTOR(SimpleAdder) {
        SC_METHOD(addNumbers);
        sensitive << a_in << b_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with SimpleAdder
    sc_signal<int> a;
    sc_signal<int> b;
    sc_signal<int> sum;

    // Instance of SimpleAdder
    SimpleAdder adder;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing inputs
        a = 11;
        b = 9;

        // Wait for one delta cycle to let the adder process
        wait(1, SC_NS);

        // Print the result
        cout << "Sum of " << a.read() << " and " << b.read() << " is: " << sum.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : adder("adder") {
        // Connecting signals to the adder ports
        adder.a_in(a);
        adder.b_in(b);
        adder.sum_out(sum);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
