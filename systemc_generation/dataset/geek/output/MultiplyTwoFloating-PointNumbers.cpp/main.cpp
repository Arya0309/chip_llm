
#include <systemc.h>

// User-defined module for multiplying two floating-point numbers
SC_MODULE(Multiplier) {
    // Input ports for two floating-point numbers
    sc_in<float> in_a, in_b;
    
    // Output port for the product of the two numbers
    sc_out<float> out_product;

    // Process to multiply two numbers
    void multiply_numbers() {
        // Reading inputs
        float a = in_a.read();
        float b = in_b.read();

        // Calculating the product
        float product = a * b;

        // Writing output
        out_product.write(product);
    }

    // Constructor to register the process
    SC_CTOR(Multiplier) {
        SC_METHOD(multiply_numbers);
        sensitive << in_a << in_b;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with Multiplier
    sc_signal<float> sig_a, sig_b;
    sc_signal<float> sig_product;

    // Instance of Multiplier
    Multiplier multiplier;

    // Process to drive inputs and print outputs
    void drive_and_print() {
        // Initializing inputs
        sig_a = 1.2;
        sig_b = 3.0;

        // Wait for one delta cycle to let the multiplier process
        wait(1, SC_NS);

        // Print the results
        cout << "Product of " << sig_a.read() << " and " << sig_b.read() << " is: " << sig_product.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : multiplier("multiplier") {
        // Connecting signals to the multiplier ports
        multiplier.in_a(sig_a);
        multiplier.in_b(sig_b);
        multiplier.out_product(sig_product);

        // Registering the drive_and_print process
        SC_THREAD(drive_and_print);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
