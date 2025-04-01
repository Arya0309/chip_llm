
#include <systemc.h>

// Module to calculate area and perimeter of a rectangle
SC_MODULE(RectangleCalculator) {
    sc_in<int> length;       // Input port for length
    sc_in<int> width;        // Input port for width
    sc_out<int> area;        // Output port for area
    sc_out<int> perimeter;   // Output port for perimeter

    // Constructor
    SC_CTOR(RectangleCalculator) {
        // Process to calculate area and perimeter
        SC_METHOD(calculate);
        sensitive << length << width;
    }

    // Method to calculate area and perimeter
    void calculate() {
        int len = length.read();
        int wid = width.read();
        area.write(len * wid);
        perimeter.write(2 * (len + wid));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> length;   // Signal for length
    sc_signal<int> width;    // Signal for width
    sc_signal<int> area;     // Signal for area
    sc_signal<int> perimeter; // Signal for perimeter

    RectangleCalculator rect_calc_inst; // Instance of the RectangleCalculator module

    // Constructor
    SC_CTOR(Testbench) : rect_calc_inst("rect_calc") {
        // Connect signals to ports
        rect_calc_inst.length(length);
        rect_calc_inst.width(width);
        rect_calc_inst.area(area);
        rect_calc_inst.perimeter(perimeter);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set initial dimensions
        length = 5;
        width = 6;
        wait(1, SC_NS); // Wait for the calculator to process

        // Print results
        cout << "Area = " << area.read() << endl;
        cout << "Perimeter = " << perimeter.read() << endl;

        // Stop the simulation
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
