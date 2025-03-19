
#include <systemc.h>

// Module to calculate area and perimeter
SC_MODULE(RectangleCalculator) {
    // Constructor
    SC_CTOR(RectangleCalculator) {
        // Process sensitivity declaration
        SC_METHOD(calculate);
        sensitive << start;
        dont_initialize();
    }

    // Input signals
    sc_in<bool> start;          // Start signal to trigger calculation
    sc_in<int> length;           // Length of the rectangle
    sc_in<int> width;            // Width of the rectangle
    
    // Output signals
    sc_out<int> area;            // Calculated area
    sc_out<int> perimeter;      // Calculated perimeter

    // Method to perform calculations
    void calculate() {
        int a = length.read();
        int b = width.read();
        area.write(a * b);                    // Calculate and write area
        perimeter.write(2 * (a + b));         // Calculate and write perimeter
    }
};

// Top module
SC_MODULE(Top) {
    // Signals
    sc_signal<bool> start;
    sc_signal<int> length;
    sc_signal<int> width;
    sc_signal<int> area;
    sc_signal<int> perimeter;

    // Instantiate RectangleCalculator module
    RectangleCalculator rect_calc;

    // Constructor
    SC_CTOR(Top) : rect_calc("rect_calc") {
        // Connect signals
        rect_calc.start(start);
        rect_calc.length(length);
        rect_calc.width(width);
        rect_calc.area(area);
        rect_calc.perimeter(perimeter);

        // Process to drive the signals
        SC_THREAD(drive);
    }

    // Thread to drive the signals
    void drive() {
        // Initialize dimensions
        length = 5;
        width = 6;
        wait(1, SC_NS); // Wait for 1 nanosecond to ensure everything is set up

        // Trigger calculation
        start = true;
        wait(1, SC_NS); // Wait for 1 nanosecond to allow calculation to complete

        // Print results
        cout << "Area = " << area.read() << endl;
        cout << "Perimeter = " << perimeter.read() << endl;
    }
};

// Simulation entry point
int sc_main(int argc, char* argv[]) {
    Top top("top");
    sc_start(); // Run simulation
    return 0;
}
