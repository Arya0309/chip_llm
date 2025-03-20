
#include <systemc.h>

// Module to calculate area and perimeter of a rectangle
SC_MODULE(RectangleCalculator) {
    // Input ports for dimensions of the rectangle
    sc_in<int> length;
    sc_in<int> width;
    
    // Output ports for area and perimeter
    sc_out<int> area;
    sc_out<int> perimeter;

    // Process to calculate area and perimeter
    void calculate() {
        // Reading inputs
        int l = length.read();
        int w = width.read();

        // Calculating area and perimeter
        int calculated_area = l * w;
        int calculated_perimeter = 2 * (l + w);

        // Writing outputs
        area.write(calculated_area);
        perimeter.write(calculated_perimeter);
    }

    // Constructor to register the process
    SC_CTOR(RectangleCalculator) {
        SC_METHOD(calculate);
        sensitive << length << width;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with RectangleCalculator
    sc_signal<int> length, width;
    sc_signal<int> calculated_area, calculated_perimeter;

    // Instance of RectangleCalculator
    RectangleCalculator rect_calc;

    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initializing dimensions of the rectangle
        length = 5;
        width = 6;

        // Wait for one delta cycle to let the calculator process
        wait(1, SC_NS);

        // Print the results
        cout << "Area = " << calculated_area.read() << endl;
        cout << "Perimeter = " << calculated_perimeter.read() << endl;
    }

    // Constructor to register the process
    SC_CTOR(Testbench) : rect_calc("rect_calc") {
        // Connecting signals to the calculator ports
        rect_calc.length(length);
        rect_calc.width(width);
        rect_calc.area(calculated_area);
        rect_calc.perimeter(calculated_perimeter);

        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
