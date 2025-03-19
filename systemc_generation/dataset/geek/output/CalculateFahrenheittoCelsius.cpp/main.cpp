
#include <systemc.h>

// Module to convert Fahrenheit to Celsius
SC_MODULE(FahrenheitToCelsius) {
    // Input port for Fahrenheit temperature
    sc_in<float> fahrenheit_in;
    
    // Output port for Celsius temperature
    sc_out<float> celsius_out;

    // Process to perform the conversion
    void convertTemperature() {
        // Read the input temperature in Fahrenheit
        float fahrenheit = fahrenheit_in.read();
        
        // Convert to Celsius using the formula
        float celsius = (fahrenheit - 32.0) * 5.0 / 9.0;
        
        // Write the output temperature in Celsius
        celsius_out.write(celsius);
    }

    // Constructor to register the process
    SC_CTOR(FahrenheitToCelsius) {
        SC_METHOD(convertTemperature);
        sensitive << fahrenheit_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signal to connect with FahrenheitToCelsius module
    sc_signal<float> fahrenheit_in;
    sc_signal<float> celsius_out;

    // Instance of FahrenheitToCelsius module
    FahrenheitToCelsius converter;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize the Fahrenheit temperature
        fahrenheit_in = 40.0;

        // Wait for one delta cycle to let the converter process
        wait(1, SC_NS);

        // Print the results
        cout << "Temperature in Fahrenheit: " << fahrenheit_in.read() << " F" << endl;
        cout << "Converted Temperature in Celsius: " << celsius_out.read() << " C" << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : converter("converter") {
        // Connecting signals to the converter ports
        converter.fahrenheit_in(fahrenheit_in);
        converter.celsius_out(celsius_out);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
