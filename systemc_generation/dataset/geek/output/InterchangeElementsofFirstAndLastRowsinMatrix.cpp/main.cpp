
#include <systemc.h>

SC_MODULE(MatrixManipulator) {
    sc_in<bool> clk; // Clock signal
    sc_in<bool> reset; // Reset signal

    SC_CTOR(MatrixManipulator) {
        SC_METHOD(interchangeFirstLast);
        sensitive << clk.pos();
        dont_initialize();

        SC_METHOD(printMatrix);
        sensitive << clk.pos();
        dont_initialize();
    }

    void interchangeFirstLast() {
        if (!reset.read()) {
            for (int i = 0; i < 4; i++) {
                int temp = matrix[i][0];
                matrix[i][0] = matrix[i][3];
                matrix[i][3] = temp;
            }
        }
    }

    void printMatrix() {
        if (!reset.read()) {
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    sc_report_handler::report(sc_core::SC_INFO, "PRINT", sc_string_concat("Element[", i, "][", j, "] = ", matrix[i][j]));
                }
                sc_report_handler::report(sc_core::SC_INFO, "PRINT", "");
            }
        }
    }

private:
    int matrix[4][4] = {{8, 9, 7, 6},
                        {4, 7, 6, 5},
                        {3, 2, 1, 8},
                        {9, 9, 7, 7}};
};

int sc_main(int argc, char* argv[]) {
    sc_clock clk("clk", 10, SC_NS, 0.5, 0, SC_NS, true);
    sc_signal<bool> reset;

    MatrixManipulator manipulator("manipulator");
    manipulator.clk(clk);
    manipulator.reset(reset);

    reset.write(true);
    sc_start(10, SC_NS); // Apply reset for 10 ns
    reset.write(false);
    sc_start(20, SC_NS); // Run for another 20 ns

    return 0;
}
