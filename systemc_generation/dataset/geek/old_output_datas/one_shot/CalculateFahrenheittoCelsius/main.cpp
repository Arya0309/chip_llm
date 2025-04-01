
#include <systemc.h>

// Module to convert Fahrenheit to Celsius
SC_MODULE(FahrenheitToCelsiusConverter) {
    sc_in<float> fahrenheit;  // Input port for Fahrenheit temperature
    sc_out<float> celsius;    // Output port for Celsius temperature

    // Constructor
    SC_CTOR(FahrenheitToCelsiusConverter) {
        // Process to perform conversion
        SC_METHOD(convert);
        sensitive << fahrenheit;
    }

    // Method to convert Fahrenheit to Celsius
    void convert() {
        float n = fahrenheit.read();
        float result = (n - 32.0) * 5.0 / 9.0;
        celsius.write(result);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<float> fahrenheit; // Signal for Fahrenheit temperature
    sc_signal<float> celsius;     // Signal for Celsius temperature

    FahrenheitToCelsiusConverter converter_inst; // Instance of the converter module

    // Constructor
    SC_CTOR(Testbench) : converter_inst("converter") {
        // Connect signals to ports
        converter_inst.fahrenheit(fahrenheit);
        converter_inst.celsius(celsius);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set input value
        fahrenheit = 40.0;
        wait(1, SC_NS); // Wait for the converter to process
        cout << "Fahrenheit: " << fahrenheit.read() << ", Celsius: " << celsius.read() << endl;

        // Additional test case
        fahrenheit = 100.0;
        wait(1, SC_NS);
        cout << "Fahrenheit: " << fahrenheit.read() << ", Celsius: " << celsius.read() << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
