
#include <systemc.h>

// Module to calculate the area and perimeter of a rectangle
SC_MODULE(RectangleCalculator) {
    sc_in<int> length;          // Input port for the length of the rectangle
    sc_in<int> width;           // Input port for the width of the rectangle
    sc_out<int> area;           // Output port for the area of the rectangle
    sc_out<int> perimeter;      // Output port for the perimeter of the rectangle

    // Constructor: Register the method to compute area and perimeter
    SC_CTOR(RectangleCalculator) {
        SC_METHOD(compute_area_perimeter);
        sensitive << length << width;
    }

    // Method to compute the area and perimeter
    void compute_area_perimeter() {
        int l = length.read();
        int w = width.read();
        area.write(l * w);
        perimeter.write(2 * (l + w));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> length_sig;  // Signal for the length of the rectangle
    sc_signal<int> width_sig;   // Signal for the width of the rectangle
    sc_signal<int> area_sig;     // Signal for the area of the rectangle
    sc_signal<int> perimeter_sig;// Signal for the perimeter of the rectangle

    // Instance of the RectangleCalculator module
    RectangleCalculator rect_calc_inst;

    // Constructor: Initialize the module and run the test thread
    SC_CTOR(Testbench)
    : rect_calc_inst("rect_calc_inst") {
        // Connect signals to module ports
        rect_calc_inst.length(length_sig);
        rect_calc_inst.width(width_sig);
        rect_calc_inst.area(area_sig);
        rect_calc_inst.perimeter(perimeter_sig);

        // Create a test thread to provide stimulus and check the result
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Set the dimensions of the rectangle
        length_sig.write(5);
        width_sig.write(6);
        wait(1, SC_NS); // Wait for the process to update

        // Read and print the results
        int calculated_area = area_sig.read();
        int calculated_perimeter = perimeter_sig.read();
        cout << "Area = " << calculated_area << endl;
        cout << "Perimeter = " << calculated_perimeter << endl;

        // End simulation after successful test
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Instantiate the testbench module
    sc_start();         // Start the simulation
    return 0;
}
