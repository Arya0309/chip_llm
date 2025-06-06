#include <systemc.h>

// Module to compute area and perimeter of a rectangle
SC_MODULE(RectangleCalculator) {
    sc_in<int> length;
    sc_in<int> width;
    sc_out<int> area;
    sc_out<int> perimeter;

    SC_CTOR(RectangleCalculator) {
        SC_METHOD(compute);
        sensitive << length << width;
        dont_initialize();
    }

    void compute() {
        int l = length.read();
        int w = width.read();
        area.write(l * w);
        perimeter.write(2 * (l + w));
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> length_sig;
    sc_signal<int> width_sig;
    sc_signal<int> area_sig;
    sc_signal<int> perimeter_sig;

    RectangleCalculator rc_inst;

    SC_CTOR(Testbench) : rc_inst("rc_inst") {
        rc_inst.length(length_sig);
        rc_inst.width(width_sig);
        rc_inst.area(area_sig);
        rc_inst.perimeter(perimeter_sig);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        // Test case: length = 5, width = 6
        length_sig = 5;
        width_sig = 6;
        wait(1, SC_NS);
        cout << "Area: " << area_sig.read() << endl;
        cout << "Perimeter: " << perimeter_sig.read() << endl;

        // Optional: add assertions
        assert(area_sig.read() == 30);
        assert(perimeter_sig.read() == 22);

        cout << "Test passed successfully." << endl;
        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
