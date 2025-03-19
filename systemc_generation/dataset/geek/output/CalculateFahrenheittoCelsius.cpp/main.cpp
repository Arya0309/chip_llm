
#include <systemc.h>

// Define a SystemC module for temperature conversion
SC_MODULE(TemperatureConverter) {
    // Input port for Fahrenheit temperature
    sc_in<float> fahrenheit;
    // Output port for Celsius temperature
    sc_out<float> celsius;
    
    // Constructor
    SC_CTOR(TemperatureConverter) {
        // Sensitivity list: react on any change in 'fahrenheit'
        SC_METHOD(convert);
        sensitive << fahrenheit;
    }

    // Method to perform conversion
    void convert() {
        // Convert Fahrenheit to Celsius and write to output port
        celsius.write((fahrenheit.read() - 32.0) * 5.0 / 9.0);
    }
};

// Testbench module
SC_MODULE(Testbench) {
    // Signal declarations for communication between modules
    sc_signal<float> fahrenheit_sig;
    sc_signal<float> celsius_sig;

    // Instance of TemperatureConverter
    TemperatureConverter converter;

    // Constructor
    SC_CTOR(Testbench) : converter("converter") {
        // Connect signals to the converter's ports
        converter.fahrenheit(fahrenheit_sig);
        converter.celsius(celsius_sig);

        // Process to initialize and monitor the test
        SC_THREAD(run);
    }

    // Thread process to run the test
    void run() {
        // Initialize Fahrenheit signal with 40 degrees
        fahrenheit_sig = 40.0;
        wait(1, SC_NS); // Wait for one nanosecond for conversion to occur

        // Print the converted Celsius temperature
        cout << "Fahrenheit: " << fahrenheit_sig.read() << " -> Celsius: " << celsius_sig.read() << endl;

        // End simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    // Create an instance of Testbench
    Testbench tb("tb");

    // Run the simulation
    sc_start();

    return 0;
}
