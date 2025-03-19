
#include <systemc.h>

// User-defined module to find roots of a quadratic equation
SC_MODULE(QuadraticSolver) {
    // Input ports for coefficients a, b, c
    sc_in<int> a_in, b_in, c_in;
    
    // Output ports for roots (real and imaginary parts)
    sc_out<double> root1_real, root1_imag, root2_real, root2_imag;
    
    // Process to calculate roots
    void calculateRoots() {
        // Reading inputs
        int a = a_in.read();
        int b = b_in.read();
        int c = c_in.read();
        
        // Check if equation is quadratic
        if (a == 0) {
            root1_real.write(std::numeric_limits<double>::quiet_NaN());
            root1_imag.write(std::numeric_limits<double>::quiet_NaN());
            root2_real.write(std::numeric_limits<double>::quiet_NaN());
            root2_imag.write(std::numeric_limits<double>::quiet_NaN());
            return;
        }
        
        // Calculate discriminant
        int d = b * b - 4 * a * c;
        double sqrt_val = sqrt(abs(d));
        
        // Determine roots based on discriminant
        if (d > 0) {
            root1_real.write((-b + sqrt_val) / (2 * a));
            root1_imag.write(0.0);
            root2_real.write((-b - sqrt_val) / (2 * a));
            root2_imag.write(0.0);
        } else if (d == 0) {
            root1_real.write(-(double)b / (2 * a));
            root1_imag.write(0.0);
            root2_real.write(-(double)b / (2 * a));
            root2_imag.write(0.0);
        } else {
            root1_real.write(-(double)b / (2 * a));
            root1_imag.write(sqrt_val / (2 * a));
            root2_real.write(-(double)b / (2 * a));
            root2_imag.write(-sqrt_val / (2 * a));
        }
    }
    
    // Constructor to register the process
    SC_CTOR(QuadraticSolver) {
        SC_METHOD(calculateRoots);
        sensitive << a_in << b_in << c_in;
    }
};

// Testbench module to drive inputs and capture outputs
SC_MODULE(Testbench) {
    // Signals to connect with QuadraticSolver
    sc_signal<int> a, b, c;
    sc_signal<double> root1_real, root1_imag, root2_real, root2_imag;
    
    // Instance of QuadraticSolver
    QuadraticSolver solver;
    
    // Process to drive inputs and print outputs
    void driveAndPrint() {
        // Initialize coefficients
        a = 1; b = -7; c = 12;
        
        // Wait for one delta cycle to let the solver process
        wait(1, SC_NS);
        
        // Print the results
        if (isnan(root1_real.read())) {
            cout << "Invalid" << endl;
        } else if (root1_imag.read() == 0 && root2_imag.read() == 0) {
            if (root1_real.read() == root2_real.read()) {
                cout << "Roots are real and same " << root1_real.read() << endl;
            } else {
                cout << "Roots are real and different " << root1_real.read() << " " << root2_real.read() << endl;
            }
        } else {
            cout << "Roots are complex " 
                 << root1_real.read() << " + i" << root1_imag.read() << " "
                 << root2_real.read() << " - i" << root2_imag.read() << endl;
        }
    }
    
    // Constructor to register the process
    SC_CTOR(Testbench) : solver("solver") {
        // Connecting signals to the solver ports
        solver.a_in(a); solver.b_in(b); solver.c_in(c);
        solver.root1_real(root1_real); solver.root1_imag(root1_imag);
        solver.root2_real(root2_real); solver.root2_imag(root2_imag);
        
        // Registering the driveAndPrint process
        SC_THREAD(driveAndPrint);
    }
};

int sc_main(int argc, char* argv[]) {
    Testbench tb("tb");
    sc_start(); // Start simulation
    return 0;
}
