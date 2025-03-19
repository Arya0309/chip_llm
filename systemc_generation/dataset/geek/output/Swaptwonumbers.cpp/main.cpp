
#include <systemc.h>

// Module to perform swapping of two numbers
SC_MODULE(Swapper) {
    // Ports for input and output
    sc_in<int> a_in, b_in;
    sc_out<int> a_out, b_out;

    // Internal variables to store the swapped values
    int a, b, temp;

    // Process to perform the swapping
    void doSwap() {
        // Read input values
        a = a_in.read();
        b = b_in.read();

        // Print initial values
        cout << "Before swapping a = " << a << " , b = " << b << endl;

        // Perform the swap using a temporary variable
        temp = a;
        a = b;
        b = temp;

        // Print swapped values
        cout << "After swapping a = " << a << " , b = " << b << endl;

        // Write output values
        a_out.write(a);
        b_out.write(b);
    }

    // Constructor to register the process
    SC_CTOR(Swapper) {
        SC_METHOD(doSwap);
        sensitive << a_in << b_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with Swapper
    sc_signal<int> a_in, b_in, a_out, b_out;

    // Instance of Swapper
    Swapper swapper;

    // Process to drive inputs
    void driveInputs() {
        // Initialize values
        a_in = 2;
        b_in = 3;

        // Wait for one delta cycle to let the swapper process
        wait(1, SC_NS);

        // Optionally, check the outputs if needed
        cout << "Captured outputs: a = " << a_out.read() << " , b = " << b_out.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : swapper("swapper") {
        // Connecting signals to the swapper ports
        swapper.a_in(a_in);
        swapper.b_in(b_in);
        swapper.a_out(a_out);
        swapper.b_out(b_out);

        // Registering the driveInputs process
        SC_THREAD(driveInputs);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
