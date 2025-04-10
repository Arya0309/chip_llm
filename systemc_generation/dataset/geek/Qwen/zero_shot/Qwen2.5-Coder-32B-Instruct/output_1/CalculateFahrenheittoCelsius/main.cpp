
#include <systemc.h>

// SystemC module to convert Fahrenheit to Celsius

SC_MODULE(TempConverter) {
    // Input and Output Ports
    sc_in<float> fahrenheit; // Input port for Fahrenheit temperature
    sc_out<float> celsius;    // Output port for Celsius temperature

    // Constructor
    SC_CTOR(TempConverter) {
        // Sensitivity list: process will trigger whenever fahrenheit changes
        SC_METHOD(convert);
        sensitive << fahrenheit;
    }

    // Process to perform the conversion
    void convert() {
        // Conversion formula: C = (F - 32) * 5/9
        float celsius_value = (fahrenheit.read() - 32.0) * 5.0 / 9.0;
        // Write the result to the celsius output port
        celsius.write(celsius_value);
    }
};

// Example SystemC top-level module for simulation

SC_MODULE(TopLevel) {
    TempConverter converter; // Instance of the temperature converter

    // Signals to connect the converter's ports
    sc_signal<float> fahrenheit_sig;
    sc_signal<float> celsius_sig;

    // Constructor
    SC_CTOR(TopLevel) : converter("converter") {
        // Connect the signals to the converter's ports
        converter.fahrenheit(fahrenheit_sig);
        converter.celsius(celsius_sig);

        // Process to initialize the input and print the output
        SC_THREAD(initialize_and_monitor);
    }

    // Thread to initialize input and monitor output
    void initialize_and_monitor() {
        // Initialize the Fahrenheit temperature
        fahrenheit_sig = 40.0;
        wait(1, SC_NS); // Wait for one nanosecond to allow conversion

        // Print the converted Celsius temperature
        cout << "Fahrenheit: " << fahrenheit_sig.read() 
             << " -> Celsius: " << celsius_sig.read() << endl;
    }
};

// Simulation entry point


SC_MODULE(Testbench) {
    sc_signal<float> fahrenheit; // Signal for Fahrenheit temperature
    sc_signal<float> celsius;    // Signal for Celsius temperature

    TempConverter converter_inst; // Instance of the TempConverter module

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
        // Test case 1: Convert 40°F to Celsius
        fahrenheit = 40.0;
        wait(1, SC_NS); // Wait for the converter to process
        float result = celsius.read();
        cout << "40°F in Celsius: " << result << "°C" << endl;
        assert(abs(result - 4.44444) < 0.0001); // Allow for floating-point precision errors

        // Test case 2: Convert 100°F to Celsius
        fahrenheit = 100.0;
        wait(1, SC_NS);
        result = celsius.read();
        cout << "100°F in Celsius: " << result << "°C" << endl;
        assert(abs(result - 37.7778) < 0.0001);

        // Test case 3: Convert 32°F to Celsius (Freezing point of water)
        fahrenheit = 32.0;
        wait(1, SC_NS);
        result = celsius.read();
        cout << "32°F in Celsius: " << result << "°C" << endl;
        assert(abs(result - 0.0) < 0.0001);

        // Test case 4: Convert 212°F to Celsius (Boiling point of water)
        fahrenheit = 212.0;
        wait(1, SC_NS);
        result = celsius.read();
        cout << "212°F in Celsius: " << result << "°C" << endl;
        assert(abs(result - 100.0) < 0.0001);

        // Print success message
        cout << "All tests passed successfully." << endl;

        sc_stop(); // Stop the simulation
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
