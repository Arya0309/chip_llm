
#include <systemc.h>

// Define a module that performs multiplication
SC_MODULE(Multiplier) {
    sc_in<float> a; // Input port for first number
    sc_in<float> b; // Input port for second number
    sc_out<float> product; // Output port for the product

    // Constructor to initialize the process
    SC_CTOR(Multiplier) {
        SC_METHOD(multiply);
        sensitive << a << b; // Sensitivity list to trigger on changes to a or b
    }

    // Process to perform multiplication
    void multiply() {
        product.write(a.read() * b.read()); // Read inputs, multiply, write to output
    }
};

// Top-level module to drive the multiplier
SC_MODULE(TopLevel) {
    sc_signal<float> a; // Signal for first number
    sc_signal<float> b; // Signal for second number
    sc_signal<float> product; // Signal for the product

    Multiplier multiplier; // Instantiate the Multiplier module

    // Constructor to connect signals and initialize values
    SC_CTOR(TopLevel) : multiplier("multiplier") {
        multiplier.a(a); // Connect signal a to input a of multiplier
        multiplier.b(b); // Connect signal b to input b of multiplier
        multiplier.product(product); // Connect signal product to output product of multiplier

        // Initialize values
        a.write(1.2);
        b.write(3.0);

        // Create a process to monitor and print the product
        SC_METHOD(print_product);
        sensitive << product; // Trigger when product changes
    }

    // Process to print the product
    void print_product() {
        cout << "Product: " << product.read() << endl;
    }
};

int sc_main(int argc, char* argv[]) {
    TopLevel top("top"); // Instantiate the top-level module
    sc_start(); // Start the simulation
    return 0;
}
