#include <systemc.h>

// PowerModule: Computes x^y using std::pow
SC_MODULE(PowerModule) {
    sc_in<double> x;       // Base
    sc_in<double> y;       // Exponent
    sc_out<double> result; // Output

    SC_CTOR(PowerModule) {
        SC_METHOD(compute_power);
        dont_initialize();
        sensitive << x << y;
    }

    void compute_power() {
        result.write(std::pow(x.read(), y.read()));
    }
};

// Testbench for PowerModule
SC_MODULE(Testbench) {
    sc_signal<double> x_sig, y_sig, result_sig;

    PowerModule power_inst;

    SC_CTOR(Testbench) : power_inst("power_inst") {
        power_inst.x(x_sig);
        power_inst.y(y_sig);
        power_inst.result(result_sig);

        SC_THREAD(run_tests);
    }

    void run_tests() {
        x_sig = 6.1;
        y_sig = 4.8;
        wait(1, SC_NS); // wait for method to evaluate

        double expected = std::pow(6.1, 4.8);
        double computed = result_sig.read();

        cout << "Result: " << computed << endl;

        // Allow small floating-point error
        assert(std::fabs(computed - expected) < 1e-6);

        cout << "Test passed successfully." << endl;

        sc_stop();
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start();
    return 0;
}
