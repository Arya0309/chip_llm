
#include <systemc.h>

// Module to calculate roots of a quadratic equation
SC_MODULE(QuadraticSolver) {
    sc_in<int> a;  // Coefficient a
    sc_in<int> b;  // Coefficient b
    sc_in<int> c;  // Coefficient c
    sc_out<sc_bv<64>> root1; // Root 1 (real part and imaginary part)
    sc_out<sc_bv<64>> root2; // Root 2 (real part and imaginary part)

    // Constructor
    SC_CTOR(QuadraticSolver) {
        SC_METHOD(calculate_roots);
        sensitive << a << b << c;
    }

    // Method to calculate roots
    void calculate_roots() {
        int a_val = a.read();
        int b_val = b.read();
        int c_val = c.read();

        if (a_val == 0) {
            // Invalid quadratic equation
            root1.write(sc_bv<64>("0"));
            root2.write(sc_bv<64>("0"));
            return;
        }

        int d = b_val * b_val - 4 * a_val * c_val;
        double sqrt_val = sqrt(abs(d));

        sc_bv<64> r1_real, r1_imag, r2_real, r2_imag;
        if (d > 0) {
            // Real and different roots
            r1_real = (double)(-b_val + sqrt_val) / (2 * a_val);
            r1_imag = 0.0;
            r2_real = (double)(-b_val - sqrt_val) / (2 * a_val);
            r2_imag = 0.0;
        } else if (d == 0) {
            // Real and same roots
            r1_real = r2_real = -(double)b_val / (2 * a_val);
            r1_imag = r2_imag = 0.0;
        } else {
            // Complex roots
            r1_real = r2_real = -(double)b_val / (2 * a_val);
            r1_imag = sqrt_val / (2 * a_val);
            r2_imag = -sqrt_val / (2 * a_val);
        }

        root1.write((sc_bv<64>)r1_real.range() + (sc_bv<64>)r1_imag.range());
        root2.write((sc_bv<64>)r2_real.range() + (sc_bv<64>)r2_imag.range());
    }
};

// Testbench module
SC_MODULE(Testbench) {
    sc_signal<int> a; // Signal for coefficient a
    sc_signal<int> b; // Signal for coefficient b
    sc_signal<int> c; // Signal for coefficient c
    sc_signal<sc_bv<64>> root1; // Signal for root 1
    sc_signal<sc_bv<64>> root2; // Signal for root 2

    QuadraticSolver solver_inst; // Instance of the QuadraticSolver module

    // Constructor
    SC_CTOR(Testbench) : solver_inst("solver") {
        // Connect signals to ports
        solver_inst.a(a);
        solver_inst.b(b);
        solver_inst.c(c);
        solver_inst.root1(root1);
        solver_inst.root2(root2);

        // Process to run tests
        SC_THREAD(run_tests);
    }

    // Thread to run test cases
    void run_tests() {
        // Initialize coefficients
        a = 1; b = -7; c = 12;
        wait(1, SC_NS); // Wait for the solver to process
        print_roots();

        // Additional test case: Complex roots
        a = 1; b = 2; c = 5;
        wait(1, SC_NS);
        print_roots();

        sc_stop(); // Stop the simulation
    }

    // Helper method to print roots
    void print_roots() {
        double r1_real = root1.read().to_double();
        double r1_imag = 0.0; // Placeholder for imaginary part
        double r2_real = root2.read().to_double();
        double r2_imag = 0.0; // Placeholder for imaginary part

        if (root1.read().range(63, 32).to_double() != 0.0) {
            r1_imag = root1.read().range(63, 32).to_double();
            r2_imag = root2.read().range(63, 32).to_double();
        }

        cout << "Root 1: " << r1_real;
        if (r1_imag != 0.0) cout << " + " << r1_imag << "i";
        cout << endl;

        cout << "Root 2: " << r2_real;
        if (r2_imag != 0.0) cout << " + " << r2_imag << "i";
        cout << endl;
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb"); // Create an instance of the Testbench

    // Start the simulation
    sc_start();

    return 0;
}
