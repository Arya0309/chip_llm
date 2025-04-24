
#include <systemc.h>

// Module that implements TempConverter function

SC_MODULE(TempConverter) {
    sc_in<float> fahrenheit;  // Input port for Fahrenheit temperature
    sc_out<float> celsius;  // Output port for Celsius temperature

    // Constructor
    SC_CTOR(TempConverter) {
        // Process to convert Fahrenheit to Celsius
        SC_METHOD(convert);
        sensitive << fahrenheit;
    }

    // Method to convert the temperature
    void convert() {
        celsius.write((fahrenheit.read() - 32.0) * 5.0 / 9.0);
    }
};

// Module that uses TempConverter function

SC_MODULE(Driver) {
    sc_in<float> fahrenheit;  // Input port for Fahrenheit temperature
    sc_out<float> celsius;  // Output port for Celsius temperature

    // Instantiate TempConverter module
    TempConverter conversion;

    // Connect input ports
    convolution.fahrenheit(fahrenheit);

    // Declare clock signal
    sc_signal<bool> clock;

    // Constructor
    SC_CTOR(Driver) {
        // Initialize clock signal
        clock = sc_get_clock_signal();

        // Process to drive TempConverter module
        SC_METHOD(drive_conversion);
        sensitive << clock;
    }

    // Drive the TempConverter module
    void drive_conversion() {
        // Assign input value to TempConverter module
        conv.fahrenheit.write(fahrenheit.read());

        // Obtain output value from TempConverter module
        celsius.write(conv.celius.read());
    }
};


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
